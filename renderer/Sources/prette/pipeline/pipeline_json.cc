#include "prette/pipeline/pipeline_json.h"

#include <fmt/format.h>
#include <string>

#include "prette/common.h"
#include "prette/json.h"
#include "prette/pipeline/pipeline_builder.h"
#include "prette/render_pass/render_pass.h"  // IWYU pragma: keep
#include "prette/shader/shader.h"
#include "prette/swapchain/swapchain.h"
#include "prette/vertex/vertex2d.h"
#include "prette/vk.h"

namespace prt::json {
PipelineHandler::~PipelineHandler() {
  for (const auto& shader : shaders_)
    delete shader;
}

void PipelineHandler::OnParseVertexShader(const std::string& value) {
  const auto shader = vk::GetVertexShader(value);
  builder()->AttachVertexShader(shader);
  shaders_.push_back(shader);
}

void PipelineHandler::OnParseFragmentShader(const std::string& value) {
  const auto shader = vk::GetFragmentShader(value);
  builder()->AttachFragmentShader(shader);
  shaders_.push_back(shader);
}

auto PipelineHandler::OnParseVertexClass(const std::string& value) -> bool {
  if (EqualsIgnoreCase(value, "tile") || EqualsIgnoreCase("color2d", value) || EqualsIgnoreCase("color2d", value)) {
    builder_->AddVertexBinding(color2d::Vertex::GetBindingDescription());
    builder_->AddVertexAttributes(color2d::Vertex::GetAttributeDescriptions());
    return TransitionTo(PipelineHandlerState::kOpenDoc);
  }
  return Error(fmt::format("Invalid vertex_class `{}`", value));
}

auto PipelineHandler::OnParseLayout(const std::string& value) -> bool {
  if (EqualsIgnoreCase(value, "default")) {
    DLOG(INFO) << "parsed layout: " << value;
  }
  return Error(fmt::format("Invalid vertex_class `{}`", value));
}

void PipelineHandler::OnParseDynamicState(const std::string& value) {
  if (value == "viewport") {
    builder()->dynamic_states_.push_back(VK_DYNAMIC_STATE_VIEWPORT);
  } else if (value == "scissor") {
    builder()->dynamic_states_.push_back(VK_DYNAMIC_STATE_SCISSOR);
  }
}

auto PipelineHandler::String(const char* str, SizeType length, bool copy) -> bool {
  const auto value = std::string(str, length);
  switch (GetState()) {
    case PipelineHandlerState::kParsingRasterizer:
      return rasterizer_.String(str, length, copy);
    case PipelineHandlerState::kParsingVertexShader:
      OnParseVertexShader(value);
      return TransitionTo(PipelineHandlerState::kOpenDoc);
    case PipelineHandlerState::kParsingFragmentShader:
      OnParseFragmentShader(value);
      return TransitionTo(PipelineHandlerState::kOpenDoc);
    case PipelineHandlerState::kParsingDynamicStates:
      OnParseDynamicState(value);
      return Continue();
    case PipelineHandlerState::kParsingExtent: {
      if (EqualsIgnoreCase(value, "swapchain")) {
        builder_->SetExtent(GetSwapchain()->GetExtent());
        return TransitionTo(PipelineHandlerState::kOpenDoc);
      }
      return Error(fmt::format("invalid extent named: {}", value));
    }
    case PipelineHandlerState::kParsingRenderPass:
      if (!pass_.String(str, length, copy))
        return false;
      if (pass_.IsClosedDoc()) {
        builder_->SetRenderPass((*pass_.GetRenderPass()));
        return TransitionTo(PipelineHandlerState::kOpenDoc);
      }
      return Continue();
    case PipelineHandlerState::kParsingLayout: {
      if (!layout_.String(str, length, copy))
        return false;
      if (layout_.IsClosedDoc()) {
        ASSERT(layout_.HasResult());
        builder_->SetPipelineLayout(layout_);
        return TransitionTo(PipelineHandlerState::kOpenDoc);
      }
      return Continue();
    }
    case PipelineHandlerState::kParsingVertexClass:
      return OnParseVertexClass(value);
    default:
      break;
  }
  return InvalidState();
}

auto PipelineHandler::StartObject() -> bool {
  switch (GetState()) {
    case PipelineHandlerState::kEmpty:
      return TransitionTo(PipelineHandlerState::kOpenDoc);
    case PipelineHandlerState::kParsingRasterizer:
      return rasterizer_.StartObject();
    case PipelineHandlerState::kParsingRenderPass:
      return pass_.StartObject();
    default:
      break;
  }
  return InvalidState();
}

auto PipelineHandler::StartArray() -> bool {
  switch (GetState()) {
    case PipelineHandlerState::kParsingDynamicStates:
      return Continue();
    default:
      break;
  }
  return InvalidState();
}

auto PipelineHandler::EndArray(SizeType elementCount) -> bool {
  switch (GetState()) {
    case PipelineHandlerState::kParsingDynamicStates:
      return TransitionTo(PipelineHandlerState::kOpenDoc);
    default:
      break;
  }
  return InvalidState();
}

auto PipelineHandler::EndObject(SizeType memberCount) -> bool {
  switch (GetState()) {
    case PipelineHandlerState::kOpenDoc:
      return TransitionToClosed();
#define DELEGATE(S, Delegate)             \
  case S: {                               \
    if (!Delegate.EndObject(memberCount)) \
      return false;                       \
    if (Delegate.IsClosedDoc())           \
      return TransitionToOpen();          \
    return Continue();                    \
  }

      DELEGATE(PipelineHandlerState::kParsingRasterizer, rasterizer_);
      DELEGATE(PipelineHandlerState::kParsingRenderPass, pass_);
    case PipelineHandlerState::kParsingVertexShader:
    case PipelineHandlerState::kParsingFragmentShader:
      return TransitionToOpen();
    default:
      break;
  }
  return InvalidState();
}

auto PipelineHandler::Key(const char* str, SizeType length, bool copy) -> bool {
  const auto key = std::string(str, length);
  switch (GetState()) {
    case PipelineHandlerState::kOpenDoc: {
      if (key.empty())
        return InvalidState();

#define FOR_EACH_PIPELINE_KEY(V)                                     \
  V("rasterizer", PipelineHandlerState::kParsingRasterizer)          \
  V("vertex_shader", PipelineHandlerState::kParsingVertexShader)     \
  V("fragment_shader", PipelineHandlerState::kParsingFragmentShader) \
  V("dynamic_states", PipelineHandlerState::kParsingDynamicStates)   \
  V("pass", PipelineHandlerState::kParsingRenderPass)                \
  V("vertex_class", PipelineHandlerState::kParsingVertexClass)       \
  V("layout", PipelineHandlerState::kParsingLayout)                  \
  V("extent", PipelineHandlerState::kParsingExtent)                  \
  V("shader_stages", PipelineHandlerState::kParsingShaderStages)

      // clang-format off
#define CHECK_KEY(Key, State) \
    else if(EqualsIgnoreCase((Key), key)) \
      return TransitionTo((State));
      // clang-format on

      FOR_EACH_PIPELINE_KEY(CHECK_KEY)
#undef CHECK_KEY
    }
    case PipelineHandlerState::kParsingRasterizer:
      return rasterizer_.Key(str, length, copy);
    case PipelineHandlerState::kParsingRenderPass:
      return pass_.Key(str, length, copy);
    default:
      break;
  }
  return InvalidState();
}
}  // namespace prt::json