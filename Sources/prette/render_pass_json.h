#ifndef PRT_RENDER_PASS_JSON_H
#define PRT_RENDER_PASS_JSON_H

#include "prette/json.h"
#include "prette/platform.h"
#include "prette/render_pass.h"

namespace prt::json {
#define FOR_EACH_RENDER_PASS_HANDLER_STATE(V) \
  V(Empty)                                    \
  V(OpenDoc)                                  \
  V(ClosedDoc)                                \
  V(Error)

enum class RenderPassHandlerState {
#define DEFINE_STATE(Name) k##Name,
  FOR_EACH_RENDER_PASS_HANDLER_STATE(DEFINE_STATE)
#undef DEFINE_STATE
};

class RenderPassHandler : public BaseStatefulReaderHandler<RenderPassHandlerState, RenderPassHandler> {
 private:
  vk::RenderPass* pass_ = nullptr;

 public:
  RenderPassHandler() :
    BaseStatefulReaderHandler<RenderPassHandlerState, RenderPassHandler>(RenderPassHandlerState::kEmpty) {}

 public:
  ~RenderPassHandler() override = default;

  auto GetRenderPass() const -> vk::RenderPass* {
    return pass_;
  }

  inline auto HasRenderPass() const -> bool {
    return IsClosedDoc() && GetRenderPass() != nullptr;
  }

#define DEFINE_STATE_CHECK(Name)                          \
  inline auto Is##Name() const->bool {                    \
    return GetState() == RenderPassHandlerState::k##Name; \
  }
  FOR_EACH_RENDER_PASS_HANDLER_STATE(DEFINE_STATE_CHECK)
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
};
}  // namespace prt::json

#endif  // PRT_RENDER_PASS_JSON_H
