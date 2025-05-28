#ifndef PRT_ASSET_REGISTRY_H
#define PRT_ASSET_REGISTRY_H

#include <filesystem>
#include <flatbuffers/flatbuffer_builder.h>
#include <functional>
#include <optional>
#include <rocksdb/db.h>
#include <rocksdb/status.h>
#include <rocksdb/write_batch.h>
#include <string>

#include "prette/assertions.h"
#include "prette/asset/asset.h"
#include "prette/common.h"
#include "prette/uri.h"

namespace prt::asset {
using AssetRegistryDB = rocksdb::DB;

class AssetIndexer {
 private:
  AssetIndexer* next_ = nullptr;

 protected:
  AssetIndexer() = default;

  auto Write(rocksdb::WriteBatch& wb, const Asset& asset, const fs::path& relpath, const uri& key) -> rocksdb::Status;

 public:
  virtual ~AssetIndexer() = default;
  virtual auto Index(const fs::path path, AssetRegistryDB* db, rocksdb::WriteBatch& wb) -> bool = 0;

  void SetNext(AssetIndexer* rhs) {
    ASSERT(rhs);
    next_ = rhs;
  }

  auto GetNext() const -> AssetIndexer* {
    return next_;
  }

  auto HasNext() const -> bool {
    return GetNext() != nullptr;
  }
};

class ShaderAssetIndexer : public AssetIndexer {
 public:
  ShaderAssetIndexer() = default;
  ~ShaderAssetIndexer() override = default;
  auto Index(const fs::path path, AssetRegistryDB* index, rocksdb::WriteBatch& wb) -> bool override;
};

class AssetRegistry {
 private:
  fs::path root_;
  AssetRegistryDB* index_ = nullptr;
  AssetIndexer* indexers_;

  auto index() const -> AssetRegistryDB* {
    return index_;
  }

 public:
  AssetRegistry(const fs::path root, AssetIndexer* indexers = nullptr);
  virtual ~AssetRegistry();
  virtual void IndexRegistry();

  auto GetRoot() const -> const fs::path& {
    return root_;
  }

  auto GetIndexPath() const -> fs::path {
    return GetRoot() / "index";
  }

  auto IsInitialized() const -> bool {
    return index() != nullptr;
  }

  auto AppendIndexer(AssetIndexer* rhs) -> AssetRegistry& {
    ASSERT(rhs);
    Append(&indexers_, rhs);
    return *this;
  }

  auto GetIndexerList() const -> AssetIndexer* {
    return indexers_;
  }

  auto GetAsset(const uri& uri) const -> std::optional<Asset>;

  auto VisitAssets(AssetVisitor* vis) -> bool;
  auto VistAllKeysAndAssets(std::function<bool(const std::string&, const Asset& asset)>) -> bool;

 public:
  static void InitRegistry(const fs::path root, AssetIndexer* indexers);
  static auto IsRegistryInitialized() -> bool;
  static auto GetRegistry() -> AssetRegistry*;
};
}  // namespace prt::asset

#endif  // PRT_ASSET_REGISTRY_H
