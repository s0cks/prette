#ifndef PRT_RASTERIZER_JSON_H
#define PRT_RASTERIZER_JSON_H

#include <ostream>

#include "prette/common.h"
#include "prette/json.h"
#include "prette/vk.h"

namespace prt::json {
#define FOR_EACH_RASTERIZER_HANDLER_STATE(V) \
  V(OpenDoc)                                 \
  V(ParsingCullMode)                         \
  V(ParsingFrontFace)                        \
  V(ParsingLineWidth)                        \
  V(ParsingPolygonMode)                      \
  V(ClosedDoc)                               \
  V(Error)

enum class RasterizerHandlerState {
#define DEFINE_STATE(Name) k##Name,
  FOR_EACH_RASTERIZER_HANDLER_STATE(DEFINE_STATE)
#undef DEFINE_STATE
};

static inline auto operator<<(std::ostream& stream, const RasterizerHandlerState& rhs) -> std::ostream& {
  switch (rhs) {
#define DEFINE_TO_STRING(Name)          \
  case RasterizerHandlerState::k##Name: \
    return stream << #Name;
    FOR_EACH_RASTERIZER_HANDLER_STATE(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
    default:
      return stream << "Unknown RasterizerHandlerState: " << static_cast<int64_t>(rhs);
  }
}

class RasterizerHandler : public BaseStatefulReaderHandler<RasterizerHandlerState, RasterizerHandler> {
 private:
  VkPipelineRasterizationStateCreateInfo* info_;

  inline auto info() const -> VkPipelineRasterizationStateCreateInfo* {
    return info_;
  }

 public:
  explicit RasterizerHandler(VkPipelineRasterizationStateCreateInfo* info) :
    BaseStatefulReaderHandler(RasterizerHandlerState::kClosedDoc),
    info_(info) {
    ASSERT(info_);
  }
  ~RasterizerHandler() override = default;

#define DEFINE_STATE_CHECK(Name)                          \
  inline auto Is##Name() const->bool {                    \
    return GetState() == RasterizerHandlerState::k##Name; \
  }
  FOR_EACH_RASTERIZER_HANDLER_STATE(DEFINE_STATE_CHECK)
#undef DEFINE_STATE_CHECK

  auto Null() -> bool override {
    return InvalidState();
  }

  auto Bool(bool b) -> bool override {
    return InvalidState();
  }

  auto Int(int i) -> bool override;
  auto Uint(unsigned u) -> bool override;
  auto Int64(int64_t i) -> bool override;
  auto Uint64(uint64_t u) -> bool override;
  auto Double(double d) -> bool override;
  auto String(const char* str, SizeType length, bool copy) -> bool override;

  auto StartObject() -> bool override {
    switch (GetState()) {
      case RasterizerHandlerState::kClosedDoc:
        return TransitionTo(RasterizerHandlerState::kOpenDoc);
      default:
        return InvalidState();
    }
  }

  auto Key(const char* str, SizeType length, bool copy) -> bool override;

  auto EndObject(SizeType memberCount) -> bool override {
    switch (GetState()) {
      case RasterizerHandlerState::kOpenDoc:
        return TransitionTo(RasterizerHandlerState::kClosedDoc);
      default:
        break;
    }
    return InvalidState();
  }

  auto StartArray() -> bool override {
    return InvalidState();
  }

  auto EndArray(SizeType elementCount) -> bool override {
    return InvalidState();
  }
};
}  // namespace prt::json

#endif  // PRT_RASTERIZER_JSON_H
