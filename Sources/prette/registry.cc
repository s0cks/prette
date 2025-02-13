#include "prette/registry.h"

#include <fmt/format.h>
#include <leveldb/options.h>
#include <leveldb/write_batch.h>

#include <algorithm>
#include <exception>
#include <filesystem>

#include "prette/common.h"
#include "prette/uri.h"

namespace prt {
static std::vector<Registry*> registries_{};

static void Register(Registry* rhs) {
  ASSERT(rhs);
  registries_.push_back(rhs);
}

static void Deregister(Registry* rhs) {
  ASSERT(rhs);
  NOT_IMPLEMENTED(ERROR);
}

auto Registry::New(const std::string& path) -> Registry* {
  ASSERT(!path.empty());
  const auto registry = new Registry(path);
  ASSERT(registry);
  Register(registry);
  return registry;
}

static inline void OpenDB(const leveldb::Options options, const std::string& filename, leveldb::DB** db) {
  const auto status = leveldb::DB::Open(options, filename, db);
  LOG_IF(FATAL, !status.ok()) << "failed to open registry index " << filename << ": " << status.ToString();
}

static inline void OpenDB(const std::string& filename, leveldb::DB** db) {
  leveldb::Options options{};
  options.create_if_missing = true;
  return OpenDB(options, filename, db);
}

Registry::Registry(std::string root) {
  ASSERT(!root.empty());
  root_ = std::move(root);
  ASSERT(fs::exists(GetRoot()) && fs::is_directory(GetRoot()));
  OpenDB(GetRoot() / "index", &db_);
  Index<ShaderIndexer>(GetRoot() / "shaders");
}

Registry::~Registry() {
  // do nothing
}

void Registry::Write(const leveldb::WriteOptions& options, leveldb::WriteBatch& batch) {
  const auto status = db_->Write(options, &batch);
  LOG_IF(FATAL, !status.ok()) << "failed to write batch to index: " << status.ToString();
}

void Registry::Init() {
  EnvironmentVariable env_path("PRT_PATH");
  std::vector<std::string> paths{};
  env_path.GetList(paths);
  for (const auto& path : paths) {
    const auto registry = Registry::New(path);
    ASSERT(registry);
  }
}

auto RegistryIndexer::HasFileExtension(const std::unordered_set<std::string>& extensions) -> PathPredicate {
  return [extensions](const fs::path& path) -> bool {
    if (!fs::is_regular_file(path))
      return false;
    const auto filename = path.filename().string();
    const auto dotpos = filename.find_first_of('.');
    const auto extension = filename.substr(dotpos);
    return extensions.find(extension) != std::end(extensions);
  };
}

template <class Iterator>
static inline void IterateWithSubscriber(const fs::path& root, rx::subscriber<fs::path>& s) {
  for (const auto& path : Iterator(root)) {
    s.on_next((const fs::path&)path);
  }
  s.on_completed();
}

auto RegistryIndexer::CreateStreamForDirectory(const fs::path& root, const bool is_recursive) -> PathStream {
  return rx::observable<>::create<fs::path>([root, is_recursive](rx::subscriber<fs::path> s) {
    if (!fs::exists(root)) {
      s.on_error(std::make_exception_ptr(std::runtime_error(fmt::format("{} does not exist", root.string()))));
      return;
    } else if (!fs::is_directory(root)) {
      s.on_error(std::make_exception_ptr(std::runtime_error(fmt::format("{} is not a directory", root.string()))));
      return;
    }
    if (is_recursive) {
      IterateWithSubscriber<fs::recursive_directory_iterator>(root, s);
    } else {
      IterateWithSubscriber<fs::directory_iterator>(root, s);
    }
  });
}

void RegistryIndexer::Index(Registry* registry) {
  leveldb::WriteBatch batch{};
  CreateStream().filter(CreateFilter()).as_blocking().subscribe([this, &batch](fs::path path) {
    IndexPath(batch, path);
  });
  registry->Write(batch);
}

// clang-format off
static const std::unordered_set<std::string> kShaderFileExtensions = {
  ".json",
  ".vert.spv",
  ".frag.spv"
};
// clang-format on

auto ShaderIndexer::CreateFilter() const -> PathPredicate {
  return HasFileExtension(kShaderFileExtensions);
}

void ShaderIndexer::IndexPath(leveldb::WriteBatch& batch, const fs::path& path) {
  const auto filename = path.filename().string();
  const auto uri = uri::Uri(fmt::format("shaders:{}", filename));
  batch.Put((std::string)uri, (std::string)path);
}
}  // namespace prt