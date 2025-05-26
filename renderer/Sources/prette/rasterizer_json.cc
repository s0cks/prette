#include "prette/rasterizer_json.h"

#include <string>

#include "prette/common.h"
#include "prette/json.h"
#include "prette/vk.h"

namespace prt::json {
static inline auto ParseCullMode(const std::string& value) -> VkCullModeFlags {
  if (value.empty() || EqualsIgnoreCase(value, "none"))
    return VK_CULL_MODE_NONE;
  else if (EqualsIgnoreCase(value, "back"))
    return VK_CULL_MODE_BACK_BIT;
  else if (EqualsIgnoreCase(value, "front"))
    return VK_CULL_MODE_FRONT_BIT;
  else if (EqualsIgnoreCase(value, "front-and-back"))
    return VK_CULL_MODE_FRONT_AND_BACK;
  LOG(FATAL) << "invalid rasterizer cull-mode: " << value;
}

static inline auto ParseFrontFace(const std::string& value) -> VkFrontFace {
  if (EqualsIgnoreCase(value, "cw") || EqualsIgnoreCase(value, "clockwise"))
    return VK_FRONT_FACE_CLOCKWISE;
  else if (EqualsIgnoreCase(value, "ccw") || EqualsIgnoreCase(value, "counter-clockwise"))
    return VK_FRONT_FACE_COUNTER_CLOCKWISE;
  LOG(FATAL) << "invalid rasterizer front-face: " << value;
}

static inline auto ParsePolygonMode(const std::string& value) -> VkPolygonMode {
  if (EqualsIgnoreCase(value, "line"))
    return VK_POLYGON_MODE_LINE;
  else if (EqualsIgnoreCase(value, "fill"))
    return VK_POLYGON_MODE_FILL;
  else if (EqualsIgnoreCase(value, "point"))
    return VK_POLYGON_MODE_POINT;
  LOG(FATAL) << "invalid rasterizer polygon mode: " << value;
}

auto RasterizerHandler::Double(double d) -> bool {
  switch (GetState()) {
    case RasterizerHandlerState::kParsingLineWidth: {
      info()->lineWidth = static_cast<float>(d);
      return TransitionTo(RasterizerHandlerState::kOpenDoc);
    }
    default:
      return InvalidState();
  }
}

auto RasterizerHandler::Int64(const int64_t v) -> bool {
  switch (GetState()) {
    case RasterizerHandlerState::kParsingLineWidth: {
      info()->lineWidth = static_cast<float>(v);
      return TransitionTo(RasterizerHandlerState::kOpenDoc);
    }
    default:
      return InvalidState();
  }
}

auto RasterizerHandler::Uint(const uint32_t v) -> bool {
  switch (GetState()) {
    case RasterizerHandlerState::kParsingLineWidth: {
      info()->lineWidth = static_cast<float>(v);
      return TransitionTo(RasterizerHandlerState::kOpenDoc);
    }
    default:
      return InvalidState();
  }
}

auto RasterizerHandler::Int(const int32_t v) -> bool {
  switch (GetState()) {
    case RasterizerHandlerState::kParsingLineWidth: {
      info()->lineWidth = static_cast<float>(v);
      return TransitionTo(RasterizerHandlerState::kOpenDoc);
    }
    default:
      return InvalidState();
  }
}

auto RasterizerHandler::Uint64(const uint64_t v) -> bool {
  switch (GetState()) {
    case RasterizerHandlerState::kParsingLineWidth: {
      info()->lineWidth = static_cast<float>(v);
      return TransitionTo(RasterizerHandlerState::kOpenDoc);
    }
    default:
      return InvalidState();
  }
}

auto RasterizerHandler::String(const char* str, SizeType length, bool copy) -> bool {
  const auto value = std::string(str, length);
  switch (GetState()) {
    case RasterizerHandlerState::kParsingCullMode: {
      info()->cullMode = ParseCullMode(value);
      return TransitionTo(RasterizerHandlerState::kOpenDoc);
    }
    case RasterizerHandlerState::kParsingFrontFace: {
      info()->frontFace = ParseFrontFace(value);
      return TransitionTo(RasterizerHandlerState::kOpenDoc);
    }
    case RasterizerHandlerState::kParsingPolygonMode: {
      info()->polygonMode = ParsePolygonMode(value);
      return TransitionTo(RasterizerHandlerState::kOpenDoc);
    }
    default:
      return InvalidState();
  }
}

#define FOR_EACH_SHADER_OBJECT_KEY(V)                       \
  V("cullMode", RasterizerHandlerState::kParsingCullMode)   \
  V("frontFace", RasterizerHandlerState::kParsingFrontFace) \
  V("lineWidth", RasterizerHandlerState::kParsingLineWidth) \
  V("polygonMode", RasterizerHandlerState::kParsingPolygonMode)

auto RasterizerHandler::Key(const char* str, SizeType length, bool copy) -> bool {
  const auto key = std::string(str, length);
  if (!IsOpenDoc())
    return InvalidState();
#define CHECK_KEY(Key, State)              \
  else if (EqualsIgnoreCase((Key), key)) { \
    return TransitionTo((State));          \
  }
  FOR_EACH_SHADER_OBJECT_KEY(CHECK_KEY);
  return InvalidState();
#undef CHECK_KEY
}
#undef FOR_EACH_SHADER_OBJECT_KEY
}  // namespace prt::json