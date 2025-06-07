#ifndef PRT_PIPELINE_LAYOUT_JSON_H
#define PRT_PIPELINE_LAYOUT_JSON_H

#include <cstdint>
#include <string>

#include "prette/assertions.h"
#include "prette/json.h"
#include "prette/vk.h"

namespace prt {
namespace vk {
class PipelineLayoutBuilder;
}

namespace json {
#define FOR_EACH_PIPELINE_LAYOUT_HANDLER_STATE(V) \
  V(Empty)                                        \
  V(OpenDoc)                                      \
  V(ParsingDescriptorSets)                        \
  V(ParsingPushConstants)                         \
  V(ClosedDoc)                                    \
  V(Error)

enum class PipelineLayoutHandlerState {
#define DEFINE_STATE(Name) k##Name,
  FOR_EACH_PIPELINE_LAYOUT_HANDLER_STATE(DEFINE_STATE)
#undef DEFINE_STATE
};

class PipelineLayoutHandler : public BaseStatefulReaderHandler<PipelineLayoutHandlerState, PipelineLayoutHandler> {
 private:
  vk::PipelineLayoutBuilder* builder_;
  vk::PipelineLayout* result_ = nullptr;

  inline void SetResult(vk::PipelineLayout* rhs) {
    ASSERT(rhs);
    result_ = rhs;
  }

  auto OnParseDescriptorSetString(const std::string& value) -> bool;
  auto OnParsePushConstantString(const std::string& value) -> bool;

#define DEFINE_TRASITION_TO(Name)                             \
  inline auto TransitionTo##Name()->bool {                    \
    return TransitionTo(PipelineLayoutHandlerState::k##Name); \
  }
  FOR_EACH_PIPELINE_LAYOUT_HANDLER_STATE(DEFINE_TRASITION_TO)
#undef DEFINE_TRASITION_TO

 public:
  explicit PipelineLayoutHandler(vk::PipelineLayoutBuilder* builder = nullptr);
  ~PipelineLayoutHandler() override = default;

  auto GetResult() const -> vk::PipelineLayout* {
    return result_;
  }

  inline auto HasResult() const -> bool {
    return GetResult() != nullptr;
  }

#define DEFINE_STATE_CHECK(Name)                              \
  inline auto Is##Name() const->bool {                        \
    return GetState() == PipelineLayoutHandlerState::k##Name; \
  }
  FOR_EACH_PIPELINE_LAYOUT_HANDLER_STATE(DEFINE_STATE_CHECK)
#undef DEFINE_STATE_CHECK

  inline auto IsClosed() const -> bool {
    return IsClosedDoc();
  }

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

  operator vk::PipelineLayout*() const {
    return GetResult();
  }
};
}  // namespace json
}  // namespace prt

#endif  // PRT_PIPELINE_LAYOUT_JSON_H
