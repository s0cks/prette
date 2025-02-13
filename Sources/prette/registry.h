#ifndef PRT_REGISTRY_H
#define PRT_REGISTRY_H

#include <leveldb/db.h>
#include <leveldb/options.h>
#include <leveldb/write_batch.h>

#include <filesystem>
#include <unordered_set>
#include <utility>

#include "prette/common.h"
#include "prette/rx.h"

namespace prt {
using PathStream = rx::observable<fs::path>;
using PathPredicate = std::function<bool(const fs::path&)>;

class Registry;
class RegistryIndexer {
 protected:
  static auto CreateStreamForDirectory(const fs::path& root, const bool is_recursive = false) -> PathStream;
  static auto HasFileExtension(const std::unordered_set<std::string>& extensions) -> PathPredicate;

 private:
  fs::path root_;

 protected:
  explicit RegistryIndexer(fs::path root) :
    root_(std::move(root)) {}

  auto GetRoot() const -> const fs::path& {
    return root_;
  }

  virtual void IndexPath(leveldb::WriteBatch& batch, const fs::path& path) = 0;

  virtual auto CreateStream() const -> PathStream {
    return CreateStreamForDirectory(GetRoot());
  }

  virtual auto CreateFilter() const -> PathPredicate = 0;

 public:
  virtual ~RegistryIndexer() = default;
  virtual void Index(Registry* registry);
};

class Registry {
  friend class RegistryIndexer;

 private:
  fs::path root_;
  leveldb::DB* db_ = nullptr;

  explicit Registry(std::string root);

  template <class Indexer>
  void Index(fs::path root) {
    Indexer indexer(root);
    return indexer.Index(this);
  }

  void Write(const leveldb::WriteOptions& options, leveldb::WriteBatch& batch);

  inline void Write(leveldb::WriteBatch& batch, const bool sync = true) {
    leveldb::WriteOptions options{};
    options.sync = sync;
    return Write(options, batch);
  }

 public:
  ~Registry();

  auto GetRoot() const -> const fs::path& {
    return root_;
  }

 private:
  static auto New(const std::string& path) -> Registry*;

 public:
  static void Init();
};

class ShaderIndexer : public RegistryIndexer {
  friend class Registry;

 public:
  struct ShaderInfo {};

 private:
  explicit ShaderIndexer(fs::path root) :
    RegistryIndexer(std::move(root)) {}

  auto CreateFilter() const -> PathPredicate override;

 protected:
  void IndexPath(leveldb::WriteBatch& batch, const fs::path& path) override;

 public:
  ~ShaderIndexer() override = default;
};
}  // namespace prt

#endif  // PRT_REGISTRY_H
