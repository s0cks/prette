#ifndef PRT_PIPELINE_JSON_H
#define PRT_PIPELINE_JSON_H

#include <fmt/format.h>
#include <glog/logging.h>
#include <ostream>
#include <string>
#include <vector>

#include "prette/common.h"
#include "prette/json.h"
#include "prette/pipeline_builder.h"
#include "prette/pipeline_layout.h"
#include "prette/rasterizer_json.h"
#include "prette/render_pass_json.h"
#include "prette/vk.h"  // IWYU pragma: keep

namespace prt {
class Shader;  // TODO: remove
}

namespace prt::json {
using namespace rapidjson;

#define FOR_EACH_PIPELINE_HANDLER_STATE(V) \
  V(Empty)                                 \
  V(OpenDoc)                               \
  V(ParsingData)                           \
  V(ParsingVertexShader)                   \
  V(ParsingFragmentShader)                 \
  V(ParsingRasterizer)                     \
  V(ParsingDynamicStates)                  \
  V(ParsingRenderPass)                     \
  V(ParsingVertexClass)                    \
  V(ParsingLayout)                         \
  V(ParsingExtent)                         \
  V(ClosedDoc)                             \
  V(Error)

enum class PipelineHandlerState {
#define DEFINE_STATE(Name) k##Name,
  FOR_EACH_PIPELINE_HANDLER_STATE(DEFINE_STATE)
#undef DEFINE_STATE
};

static inline auto operator<<(std::ostream& stream, const PipelineHandlerState& rhs) -> std::ostream& {
  switch (rhs) {
#define DEFINE_TO_STRING(Name)        \
  case PipelineHandlerState::k##Name: \
    return stream << #Name;

    FOR_EACH_PIPELINE_HANDLER_STATE(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
    default:
      return stream << "invalid state.";
  }
}

class PipelineHandler : public BaseStatefulReaderHandler<PipelineHandlerState, PipelineHandler> {
  friend class RenderPipeline;

  struct KeyState {
    const char* name;
    PipelineHandlerState state;

    auto operator==(const std::string& rhs) const -> bool {
      return name == rhs;
    }

    operator PipelineHandlerState() const {
      return state;
    }
  };

 private:
  vk::BaseRenderPipelineBuilder* builder_;
  std::vector<vk::Shader*> shaders_{};
  RasterizerHandler rasterizer_;
  RenderPassHandler pass_{};
  PipelineLayoutHandler layout_{};

  inline auto builder() const -> vk::BaseRenderPipelineBuilder* {
    return builder_;
  }

  inline auto Continue() -> bool {
    return true;
  }

  inline auto Error(const std::string& rhs) -> bool {
    SetState(PipelineHandlerState::kError);
    SetError(rhs);
    return false;
  }

  inline auto InvalidState() -> bool {
    return Error("Invalid State");
  }

  inline auto TransitionTo(const PipelineHandlerState rhs) -> bool {
    SetState(rhs);
    return Continue();
  }

  inline auto TransitionToOpen() -> bool {
    return TransitionTo(PipelineHandlerState::kOpenDoc);
  }

  inline auto TransitionToClosed() -> bool {
    return TransitionTo(PipelineHandlerState::kClosedDoc);
  }

  template <typename Container>
  inline auto CheckKeyTable(const std::string& key, const Container& key_table) -> bool {
    for (const auto& entry : key_table) {
      if (key == entry)
        return TransitionTo(entry);
    }
    return Error(fmt::format("invalid key `{}`", key));
  }

  void OnParseVertexShader(const std::string& value);
  void OnParseFragmentShader(const std::string& value);
  void OnParseDynamicState(const std::string& value);
  auto OnParseVertexClass(const std::string& value) -> bool;
  auto OnParseLayout(const std::string& value) -> bool;

 public:
  explicit PipelineHandler(vk::BaseRenderPipelineBuilder* builder) :
    BaseStatefulReaderHandler<PipelineHandlerState, PipelineHandler>(PipelineHandlerState::kEmpty),
    builder_(builder),
    rasterizer_(&builder->rasterizer_) {}
  ~PipelineHandler() override;

#define DEFINE_STATE_CHECK(Name)                        \
  inline auto Is##Name() const->bool {                  \
    return GetState() == PipelineHandlerState::k##Name; \
  }
  FOR_EACH_PIPELINE_HANDLER_STATE(DEFINE_STATE_CHECK)
#undef DEFINE_STATE_CHECK

  inline auto IsClosed() const -> bool {
    return IsClosedDoc();
  }

  auto Null() -> bool override {
    return InvalidState();
  }

  auto Bool(bool b) -> bool override {
    return InvalidState();
  }

  auto Int(int i) -> bool override {
    return InvalidState();
  }

  auto Uint(unsigned u) -> bool override {
    return InvalidState();
  }

  auto Int64(int64_t i) -> bool override {
    return InvalidState();
  }

  auto Uint64(uint64_t u) -> bool override {
    return InvalidState();
  }

  auto Double(double d) -> bool override {
    return InvalidState();
  }

  auto Key(const char* str, SizeType length, bool copy) -> bool override;
  auto String(const char* str, SizeType length, bool copy) -> bool override;
  auto StartObject() -> bool override;
  auto EndObject(SizeType memberCount) -> bool override;
  auto StartArray() -> bool override;
  auto EndArray(SizeType elementCount) -> bool override;
};
}  // namespace prt::json

#endif  // PRT_PIPELINE_JSON_H
