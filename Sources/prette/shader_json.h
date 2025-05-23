#ifndef PRT_SHADER_JSON_H
#define PRT_SHADER_JSON_H

#include <ostream>

#include "prette/common.h"
#include "prette/json.h"

namespace prt::json {
#define FOR_EACH_SHADER_OBJECT_HANDLER_STATE(V) \
  V(Open)                                       \
  V(ParsingCode)                                \
  V(Closed)                                     \
  V(Error)

enum class ShaderObjectHandlerState {
#define DEFINE_STATE(Name) k##Name,
  FOR_EACH_SHADER_OBJECT_HANDLER_STATE(DEFINE_STATE)
#undef DEFINE_STATE
};

static inline auto operator<<(std::ostream& stream, const ShaderObjectHandlerState& rhs) -> std::ostream& {
  switch (rhs) {
#define DEFINE_TO_STRING(Name)            \
  case ShaderObjectHandlerState::k##Name: \
    return stream << #Name;
    FOR_EACH_SHADER_OBJECT_HANDLER_STATE(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
    default:
      return stream << "Unknown ShaderObjectHandlerState: " << static_cast<int64_t>(rhs);
  }
}

class ShaderObjectHandler : public BaseStatefulReaderHandler<ShaderObjectHandlerState, ShaderObjectHandler> {
 protected:
  auto OnParseCode(fs::path file) -> bool;

 public:
  ShaderObjectHandler() :
    BaseStatefulReaderHandler(ShaderObjectHandlerState::kOpen) {}
  ~ShaderObjectHandler() = default;

#define DEFINE_STATE_CHECK(Name)                            \
  inline auto Is##Name() const->bool {                      \
    return GetState() == ShaderObjectHandlerState::k##Name; \
  }
  FOR_EACH_SHADER_OBJECT_HANDLER_STATE(DEFINE_STATE_CHECK)
#undef DEFINE_STATE_CHECK

  auto Null() -> bool {
    return InvalidState();
  }

  auto Bool(bool b) -> bool {
    return InvalidState();
  }

  auto Int(int i) -> bool {
    return InvalidState();
  }

  auto Uint(unsigned u) -> bool {
    return InvalidState();
  }

  auto Int64(int64_t i) -> bool {
    return InvalidState();
  }

  auto Uint64(uint64_t u) -> bool {
    return InvalidState();
  }

  auto Double(double d) -> bool {
    return InvalidState();
  }

  auto String(const char* str, SizeType length, bool copy) -> bool;

  auto StartObject() -> bool {
    switch (GetState()) {
      case ShaderObjectHandlerState::kClosed:
        return TransitionTo(ShaderObjectHandlerState::kOpen);
      default:
        return InvalidState();
    }
  }

  auto Key(const char* str, SizeType length, bool copy) -> bool;

  auto EndObject(SizeType memberCount) -> bool {
    switch (GetState()) {
      case ShaderObjectHandlerState::kOpen:
        return TransitionTo(ShaderObjectHandlerState::kClosed);
      default:
        break;
    }
    return InvalidState();
  }

  auto StartArray() -> bool {
    return InvalidState();
  }

  auto EndArray(SizeType elementCount) -> bool {
    return InvalidState();
  }
};
}  // namespace prt::json

#endif  // PRT_SHADER_JSON_H
