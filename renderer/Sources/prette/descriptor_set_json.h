#ifndef PRT_DESCRIPTOR_SET_JSON_H
#define PRT_DESCRIPTOR_SET_JSON_H

#include <ostream>

#include "prette/common.h"
#include "prette/json.h"

namespace prt::json {
#define FOR_EACH_DESCRIPTOR_SET_HANDLER_STATE(V) \
  V(Empty)                                       \
  V(OpenDoc)                                     \
  V(ParsingBindings)                             \
  V(ClosedDoc)                                   \
  V(Error)

enum DescriptorSetHandlerState {
#define DEFINE_STATE(Name) k##Name,
  FOR_EACH_DESCRIPTOR_SET_HANDLER_STATE(DEFINE_STATE)
#undef DEFINE_STATE
};

static inline auto operator<<(std::ostream& stream, const DescriptorSetHandlerState rhs) -> std::ostream& {
  switch (rhs) {
#define DEFINE_TOSTRING(Name)              \
  case DescriptorSetHandlerState::k##Name: \
    return stream << #Name;
    FOR_EACH_DESCRIPTOR_SET_HANDLER_STATE(DEFINE_TOSTRING)
#undef DEFINE_TOSTRING
    default:
      return stream << "unknown DescriptorHandlerState: " << static_cast<int64_t>(rhs);
  }
}

class DescriptorSetHandler : public BaseStatefulReaderHandler<DescriptorSetHandlerState, DescriptorSetHandler> {
 public:
  DescriptorSetHandler() :
    BaseStatefulReaderHandler<DescriptorSetHandlerState, DescriptorSetHandler>(DescriptorSetHandlerState::kEmpty) {}
  ~DescriptorSetHandler() override = default;
  auto Null() -> bool override;
  auto Bool(bool b) -> bool override;
  auto Int(int i) -> bool override;
  auto Uint(unsigned u) -> bool override;
  auto Int64(int64_t i) -> bool override;
  auto Uint64(uint64_t u) -> bool override;
  auto Double(double d) -> bool override;
  auto Key(const char* str, SizeType length, bool copy) -> bool override;
  auto String(const char* str, SizeType length, bool copy) -> bool override;
  auto StartObject() -> bool override;
  auto EndObject(SizeType memberCount) -> bool override;
  auto StartArray() -> bool override;
  auto EndArray(SizeType elementCount) -> bool override;
};
}  // namespace prt::json

#endif  // PRT_DESCRIPTOR_SET_JSON_H
