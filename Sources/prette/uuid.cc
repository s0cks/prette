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

  int UUID::Compare(const UUID& lhs, const UUID& rhs) {
    return uuid_compare(lhs.raw(), rhs.raw());
  }

  std::string UUID::ToString() const {
    static constexpr const auto kUUIDStringLength = 37;

    uuid_string_t ss;
    uuid_unparse(raw_, ss);
    return std::string(ss, kUUIDStringLength);
  }
#else
#error "Unsupported Platform."
#endif
}