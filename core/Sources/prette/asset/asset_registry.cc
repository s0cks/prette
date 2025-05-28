#include "prette/asset/asset_registry.h"

#include <chrono>
#include <filesystem>
#include <flatbuffers/flatbuffer_builder.h>
#include <fmt/format.h>
#include <functional>
#include <optional>
#include <ostream>
#include <rocksdb/iterator.h>
#include <rocksdb/options.h>
#include <rocksdb/status.h>
#include <rocksdb/write_batch.h>
#include <string>

#include "prette/assertions.h"
#include "prette/asset/asset.h"
#include "prette/asset/asset_type.h"
#include "prette/asset/content_type.h"
#include "prette/asset_generated.h"
#include "prette/common.h"
#include "prette/thread_local.h"

namespace prt::asset {
static ThreadLocal<AssetRegistry> registry_{};

static inline auto operator<<(std::ostream& stream, const rocksdb::Status& rhs) -> std::ostream& {
  if (rhs.ok())
    return stream << "ok.";
  return stream << rhs.ToString();
}

AssetRegistry::AssetRegistry(const fs::path root, AssetIndexer* indexers) :
  root_(root),
  indexers_(indexers) {
  const auto is_genesis = !fs::exists(GetIndexPath());
  DLOG(INFO) << (is_genesis ? "creating" : "opening") << " AssetRegistry for: " << root;
  rocksdb::Options options{};
  options.create_if_missing = true;
  const auto status = AssetRegistryDB::Open(options, GetIndexPath(), &index_);
  LOG_IF(FATAL, !status.ok()) << "failed to open asset index: " << status;
  if (is_genesis)
    IndexRegistry();
}

AssetRegistry::~AssetRegistry() {
  delete index_;
}

void AssetRegistry::IndexRegistry() {
  rocksdb::WriteBatch write_batch{};
  auto indexer = GetIndexerList();
  PRT_PROFILING_BEGIN(index_all);
  while (indexer != nullptr) {
    indexer->Index(GetRoot(), index(), write_batch);
    indexer = indexer->GetNext();
  }
  {
    rocksdb::WriteOptions options{};
    options.sync = true;
    const auto status = index()->Write(options, &write_batch);
    LOG_IF(FATAL, !status.ok()) << "failed to write index batch: " << status.ToString();
  }
  {
    rocksdb::FlushOptions options{};
    const auto status = index()->Flush(options);
    LOG_IF(FATAL, !status.ok()) << "failed to flush index: " << status.ToString();
  }
  PRT_PROFILING_END(index_all);
  DVLOG(1) << GetRoot() << " indexed in: " << std::chrono::duration_cast<std::chrono::milliseconds>(index_all_duration);
}

void AssetRegistry::InitRegistry(const fs::path root, AssetIndexer* indexers) {
  ASSERT(!IsRegistryInitialized());
  registry_ = new AssetRegistry(root, indexers);
}

auto AssetRegistry::IsRegistryInitialized() -> bool {
  return registry_.Get() != nullptr;
}

auto AssetRegistry::GetRegistry() -> AssetRegistry* {
  ASSERT(IsRegistryInitialized());
  return registry_.Get();
}

static inline auto HasExtension(const char* extension) -> std::function<bool(const fs::path&)> {
  ASSERT(extension);
  return [extension](const fs::path& p) {
    return p.extension() == extension;
  };
}

static constexpr const auto kJsonExtension = ".json";
static const auto HasJsonExtension = HasExtension(kJsonExtension);

static constexpr const auto kVertexSpirvExtension = ".vspv";
static const auto HasVertexSpirvExtension = HasExtension(kVertexSpirvExtension);

static constexpr const auto kFragmentSpirvExtension = ".fspv";
static const auto HasFragmentSpirvExtension = HasExtension(kFragmentSpirvExtension);

static constexpr const auto kGlslExtension = ".glsl";
static const auto HasGlslExtension = HasExtension(kGlslExtension);

static constexpr const auto kFragSourceExtension = ".frag";
static const auto HasFragSourceExtension = HasExtension(kFragSourceExtension);

static constexpr const auto kVertexSourceExtension = ".vert";
static const auto HasVertexSourceExtension = HasExtension(".vert");

static inline auto IsValidShaderFile(const fs::directory_entry& rhs) -> bool {
  if (!rhs.is_regular_file())
    return false;
  const auto& path = rhs.path();
  return HasJsonExtension(path) || HasVertexSpirvExtension(path) || HasFragmentSpirvExtension(path) ||
         HasGlslExtension(path) || HasVertexSourceExtension(path) || HasFragSourceExtension(path);
}

auto AssetIndexer::Write(rocksdb::WriteBatch& wb, const Asset& asset, const fs::path& relpath, const uri& key)
    -> rocksdb::Status {
  flatbuffers::FlatBufferBuilder fbb{};
  const auto path_offset = fbb.CreateString(relpath.c_str());
  asset::raw::AssetBuilder ab(fbb);
  ab.add_content_type(static_cast<raw::ContentType>(asset.GetContentType()));
  ab.add_type(static_cast<raw::AssetType>(asset.GetType()));
  ab.add_path(path_offset);
  const auto raw_asset = ab.Finish();
  fbb.Finish(raw_asset);

  const rocksdb::Slice k(key.data());
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
  const rocksdb::Slice v((const char*)fbb.GetBufferPointer(), fbb.GetSize());
  return wb.Put(k, v);
}

auto ShaderAssetIndexer::Index(const fs::path root, AssetRegistryDB* index, rocksdb::WriteBatch& wb) -> bool {
  ASSERT(index);
  const auto shaders_dir = root / "shaders";
  if (!fs::exists(shaders_dir)) {
    LOG(WARNING) << "shaders directory " << shaders_dir << " doesn't exist.";
    return false;
  } else if (!fs::is_directory(shaders_dir)) {
    LOG(WARNING) << shaders_dir << " is not a directory.";
    return false;
  }

  for (const auto& dir_entry : fs::directory_iterator(shaders_dir)) {
    if (!IsValidShaderFile(dir_entry))
      continue;
    const fs::path& path = dir_entry;
    AssetType asset_type = kShaderAssetType;
    ContentType content_type = kDefaultContentType;
    if (HasVertexSourceExtension(path)) {
      content_type = ContentType::kTextGlslVertexShader;
    } else if (HasFragSourceExtension(path)) {
      content_type = ContentType::kTextGlslFragmentShader;
    } else if (HasVertexSpirvExtension(path)) {
      content_type = ContentType::kSpirvVertexShader;
    } else if (HasFragmentSpirvExtension(path)) {
      content_type = ContentType::kSpirvFragmentShader;
    } else if (HasJsonExtension(path)) {
      content_type = ContentType::kApplicationJson;
    } else if (HasGlslExtension(path)) {
      content_type = ContentType::kTextGlsl;
    }

    const auto relpath = fs::relative(path, root).string();
    const auto key = uri(fmt::format("shader:{}", fs::relative(path, shaders_dir).c_str()));
    Asset asset(asset_type, content_type, relpath);
    DLOG(INFO) << "indexing " << key << " => " << asset << ".....";

    flatbuffers::FlatBufferBuilder fbb{};
    const auto path_offset = fbb.CreateString(relpath.c_str());
    asset::raw::AssetBuilder ab(fbb);
    ab.add_content_type(static_cast<raw::ContentType>(asset.GetContentType()));
    ab.add_type(static_cast<raw::AssetType>(asset.GetType()));
    ab.add_path(path_offset);
    const auto raw_asset = ab.Finish();
    fbb.Finish(raw_asset);

    const rocksdb::Slice k(key.data());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
    const rocksdb::Slice v((const char*)fbb.GetBufferPointer(), fbb.GetSize());

    const auto status = index->Put(rocksdb::WriteOptions{}, k, v);
    LOG_IF(FATAL, !status.ok()) << "failed to index " << key << " => " << asset;
  }
  return true;
}

static inline auto Decode(const rocksdb::Slice& value) -> Asset {
  return {*raw::GetAsset(value.data())};
}

auto AssetRegistry::VistAllKeysAndAssets(std::function<bool(const std::string&, const Asset& asset)> vis) -> bool {
  rocksdb::ReadOptions options{};
  rocksdb::Iterator* iter = index()->NewIterator(options);
  for (iter->SeekToFirst(); iter->Valid(); iter->Next()) {
    Asset asset = Decode(iter->value());
    if (!vis(iter->key().ToString(), asset))
      return false;
  }
  delete iter;
  return true;
}

auto AssetRegistry::VisitAssets(AssetVisitor* vis) -> bool {
  ASSERT(vis);
  rocksdb::ReadOptions options{};
  rocksdb::Iterator* iter = index()->NewIterator(options);
  for (iter->SeekToFirst(); iter->Valid(); iter->Next()) {
    const auto key = iter->key();
    Asset asset = Decode(iter->value());
    if (!vis->Visit(&asset))
      return false;
  }
  delete iter;
  return true;
}

auto AssetRegistry::GetAsset(const uri& uri) const -> std::optional<Asset> {
  std::string value{};
  rocksdb::ReadOptions options{};
  const auto status = index()->Get(options, uri.data(), &value);
  if (status.IsNotFound())
    return {};
  LOG_IF(FATAL, !status.ok()) << "failed to find Asset " << uri << ": " << status.ToString();
  const auto asset = raw::GetAsset(value.data());
  return {Asset(*asset)};
}
}  // namespace prt::asset