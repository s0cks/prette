#ifndef PRT_ASSET_H
#define PRT_ASSET_H

#include <array>
#include <ostream>
#include <string>

#include "prette/assertions.h"
#include "prette/asset/asset_type.h"
#include "prette/asset/content_type.h"
#include "prette/common.h"

// IWYU pragma: begin_exports
#include "prette/asset_generated.h"
#include "prette/uri.h"
// IWYU pragma: end_exports

namespace prt::asset {
using AssetHash = std::array<uint8_t, 0>;

class Asset;
class AssetVisitor {
 protected:
  AssetVisitor() = default;

 public:
  virtual ~AssetVisitor() = default;
  virtual auto Visit(Asset* rhs) -> bool = 0;
};

class AssetRegistry;
class Asset {
  DEFINE_DEFAULT_COPYABLE_TYPE(Asset);

 private:
  AssetType type_ = kInvalidAssetType;
  ContentType content_type_ = kInvalidContentType;
  fs::path path_{};
  AssetHash hash_{};

 public:
  Asset() = default;
  Asset(const AssetType type, const ContentType content_type, const fs::path path, const AssetHash hash = AssetHash()) :
    type_(type),
    content_type_(content_type),
    path_(path),
    hash_(hash) {}
  Asset(const raw::Asset& raw);
  ~Asset() = default;

  auto GetType() const -> const AssetType& {
    return type_;
  }

  auto GetPath() const -> const fs::path& {
    return path_;
  }

  auto GetContentType() const -> const ContentType& {
    return content_type_;
  }

  auto GetHash() const -> const AssetHash& {
    return hash_;
  }

#define DEFINE_TYPE_CHECK(Name)                        \
  inline auto Is##Name() const->bool {                 \
    return GetType() == AssetType::k##Name##AssetType; \
  }
  FOR_EACH_ASSET_TYPE(DEFINE_TYPE_CHECK)
#undef DEFINE_TYPE_CHECK

  inline auto Accept(AssetVisitor* vis) -> bool {
    ASSERT(vis);
    return vis->Visit(this);
  }

  auto ToString() const -> std::string;

  friend auto operator<<(std::ostream& stream, const Asset& rhs) -> std::ostream& {
    return stream << rhs.ToString();
  }
};
}  // namespace prt::asset

#endif  // PRT_ASSET_H
