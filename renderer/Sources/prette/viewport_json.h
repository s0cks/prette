#ifndef PRT_VIEWPORT_JSON_H
#define PRT_VIEWPORT_JSON_H

#include <cstdint>

#include "prette/json.h"

namespace prt::json {

#define FOR_EACH_VIEWPORT_HANDLER_STATE(V) \
  V(Empty)                                 \
  V(OpenDoc)                               \
  V(ParsingX)                              \
  V(ParsingY)                              \
  V(ParsingWidth)                          \
  V(ParsingHeight)                         \
  V(ParsingMinDepth)                       \
  V(ParsingMaxDepth)                       \
  V(ClosedDoc)                             \
  V(Error)

enum class ViewportReaderHandlerState {
#define DEFINE_STATE(Name) k##Name,
  FOR_EACH_VIEWPORT_HANDLER_STATE(DEFINE_STATE)
#undef DEFINE_STATE
};

class ViewportReaderHandler : public BaseStatefulReaderHandler<ViewportReaderHandlerState, ViewportReaderHandler> {
 private:
  VkViewport viewport_{};

 public:
  ViewportReaderHandler() :
    BaseStatefulReaderHandler<ViewportReaderHandlerState, ViewportReaderHandler>(ViewportReaderHandlerState::kEmpty) {}
  ~ViewportReaderHandler() override = default;

#define DEFINE_STATE_CHECK(Name)                              \
  inline auto Is##Name() const->bool {                        \
    return GetState() == ViewportReaderHandlerState::k##Name; \
  }
  FOR_EACH_VIEWPORT_HANDLER_STATE(DEFINE_STATE_CHECK)
#undef DEFINE_STATE_CHECK

  auto Key(const char* str, SizeType length, bool copy) -> bool override;
  auto String(const char* str, SizeType length, bool copy) -> bool override;
  auto StartObject() -> bool override;
  auto EndObject(SizeType memberCount) -> bool override;
  auto StartArray() -> bool override;
  auto EndArray(SizeType elementCount) -> bool override;
  auto Null() -> bool override;
  auto Bool(bool b) -> bool override;
  auto Int(int i) -> bool override;
  auto Uint(unsigned u) -> bool override;
  auto Int64(int64_t i) -> bool override;
  auto Uint64(uint64_t u) -> bool override;
  auto Double(double d) -> bool override;

  operator VkViewport() const {
    return viewport_;
  }
};
}  // namespace prt::json

#endif  // PRT_VIEWPORT_JSON_H
