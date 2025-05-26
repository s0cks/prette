#include "prette/settings/setting_type.h"

#include "prette/assertions.h"
#include "prette/common.h"

namespace prt {
auto BoolSetting::operator()(const uint8_t* bytes, const uint64_t num_bytes) -> bool {
  ASSERT(bytes && num_bytes == kValueSizeInBytes);
  return *((bool*)bytes);  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
}
}  // namespace prt