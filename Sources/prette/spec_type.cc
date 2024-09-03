#include "prette/spec_type.h"

namespace prt::spec {
  constexpr const char* ToString(const Type& rhs) {
    switch(rhs) {
#define DEFINE_TO_STRING(Name)  \
      case k##Name: return #Name;
      FOR_EACH_SPEC_TYPE(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
      default: return "Unknown spec::Type";
    }
  }

  static inline bool
  IsType(const char* expected, const uint64_t expected_length,
         const char* actual, const uint64_t actual_length) {
    return actual_length == expected_length
        && (strncmp(expected, actual, expected_length) == 0);
  }

#define DEFINE_TYPE_CHECK(Name)                                 \
  static const uint64_t k##Name##Length = strlen(#Name);        \
  static inline bool                                            \
  Is##Name##Type(const char* value, const uint64_t length) {    \
    return IsType(#Name, k##Name##Length, value, length);       \
  }

  FOR_EACH_SPEC_TYPE(DEFINE_TYPE_CHECK)
#undef DEFINE_TYPE_CHECK

  std::optional<Type> Parse(const char* value, const uint64_t length) {
    if(!value || length <= 0)
      return std::nullopt;
#define DEFINE_PARSE_TYPE(Name)                               \
    else if(Is##Name##Type(value, length))                    \
      return { Type::k##Name };
    FOR_EACH_SPEC_TYPE(DEFINE_PARSE_TYPE)
#undef DEFINE_PARSE_TYPE
    return std::nullopt;
  }
}