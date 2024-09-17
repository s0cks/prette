#include "prette/uuid.h"

namespace prt {
#ifdef OS_IS_OSX
  void UUID::Generate(RawType& dst) {
    uuid_generate(dst);
  }

  void UUID::ParseFrom(const char* src) {
    uuid_parse(src, raw_);
  }

  void UUID::CopyFrom(const RawType& src) {
    uuid_copy(raw_, src);
  }

  auto UUID::Compare(const UUID& lhs, const UUID& rhs) -> int {
    return uuid_compare(lhs.raw(), rhs.raw());
  }

  auto UUID::ToString() const -> std::string {
    static constexpr const auto kUUIDStringLength = 37;

    uuid_string_t ss;
    uuid_unparse(raw_, ss);
    return { ss, kUUIDStringLength };
  }
#else
#error "Unsupported Platform."
#endif
}