#ifndef PRT_ASSET_TYPE_H
#define PRT_ASSET_TYPE_H

#include <cstdint>
#include <ostream>

namespace prt {
#define FOR_EACH_ASSET_TYPE(V) \
  V(Shader)                    \
  V(Texture)                   \
  V(Mesh)                      \
  V(Sound)                     \
  V(Material)                  \
  V(Font)                      \
  V(Binary)

enum AssetType : uint8_t {
  kInvalidAssetType = 0,
#define DEFINE(Name) k##Name##AssetType,
  FOR_EACH_ASSET_TYPE(DEFINE)
#undef DEFINE
  // clang-format off
  kTotalNumberOfAssetTypes,
  kDefaultAssetType = kBinaryAssetType,
  // clang-format on
};

static inline auto operator<<(std::ostream& stream, const AssetType& rhs) -> std::ostream& {
  switch (rhs) {
#define DEFINE_TOSTRING(Name)         \
  case AssetType::k##Name##AssetType: \
    return stream << #Name;
    FOR_EACH_ASSET_TYPE(DEFINE_TOSTRING);
#undef DEFINE_TOSTRING
    case AssetType::kInvalidAssetType:
    default:
      return stream << "Invalid AssetType: " << static_cast<uint64_t>(rhs);
  }
}
}  // namespace prt

#endif  // PRT_ASSET_TYPE_H
