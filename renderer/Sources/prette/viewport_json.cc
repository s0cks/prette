#include "prette/viewport_json.h"

#include <string>

#include "prette/common.h"
#include "prette/gfx.h"
#include "prette/gfx_vk.h"
#include "prette/json.h"
#include "prette/swapchain/swapchain.h"

namespace prt::json {

auto ViewportReaderHandler::Key(const char* str, SizeType length, bool copy) -> bool {
  std::string key(str, length);
  if (EqualsIgnoreCase(key, "x"))
    return TransitionTo(ViewportReaderHandlerState::kParsingX);
  else if (EqualsIgnoreCase(key, "y"))
    return TransitionTo(ViewportReaderHandlerState::kParsingY);
  else if (EqualsIgnoreCase(key, "width"))
    return TransitionTo(ViewportReaderHandlerState::kParsingWidth);
  else if (EqualsIgnoreCase(key, "height"))
    return TransitionTo(ViewportReaderHandlerState::kParsingHeight);
  else if (EqualsIgnoreCase(key, "min_depth"))
    return TransitionTo(ViewportReaderHandlerState::kParsingMinDepth);
  else if (EqualsIgnoreCase(key, "max_depth"))
    return TransitionTo(ViewportReaderHandlerState::kParsingMaxDepth);
  return InvalidState();
}

auto ViewportReaderHandler::String(const char* str, SizeType length, bool copy) -> bool {
  if (!IsEmpty())
    return InvalidState();
  std::string value(str, length);
  if (EqualsIgnoreCase(value, "full")) {
    const auto& extent = GetSwapchain()->GetExtent();
    viewport_ = VkViewport{
        .x = 0,
        .y = 0,
        .width = static_cast<float>(extent.width),
        .height = static_cast<float>(extent.height),
        .minDepth = prt::vk::kDefaultViewportMinDepth,
        .maxDepth = prt::vk::kDefaultViewportMaxDepth,
    };
    return TransitionTo(ViewportReaderHandlerState::kClosedDoc);
  }
  return InvalidState();
}

auto ViewportReaderHandler::StartObject() -> bool {
  if (IsEmpty() || IsClosedDoc())
    return TransitionTo(ViewportReaderHandlerState::kOpenDoc);
  return InvalidState();
}

auto ViewportReaderHandler::EndObject(SizeType memberCount) -> bool {
  return InvalidState();
}

auto ViewportReaderHandler::StartArray() -> bool {
  return InvalidState();
}

auto ViewportReaderHandler::EndArray(SizeType elementCount) -> bool {
  return InvalidState();
}

auto ViewportReaderHandler::Null() -> bool {
  return InvalidState();
}

auto ViewportReaderHandler::Bool(bool b) -> bool {
  return InvalidState();
}

auto ViewportReaderHandler::Int(int i) -> bool {
  return InvalidState();
}

auto ViewportReaderHandler::Uint(unsigned u) -> bool {
  switch (GetState()) {
    case ViewportReaderHandlerState::kParsingX:
      viewport_.x = static_cast<float>(u);
      break;
    case ViewportReaderHandlerState::kParsingY:
      viewport_.y = static_cast<float>(u);
      break;
    case ViewportReaderHandlerState::kParsingWidth:
      viewport_.width = static_cast<float>(u);
      break;
    case ViewportReaderHandlerState::kParsingHeight:
      viewport_.height = static_cast<float>(u);
      break;
    case ViewportReaderHandlerState::kParsingMinDepth:
      viewport_.minDepth = static_cast<float>(u);
      break;
    case ViewportReaderHandlerState::kParsingMaxDepth:
      viewport_.maxDepth = static_cast<float>(u);
      break;
    default:
      return InvalidState();
  }
  return TransitionTo(ViewportReaderHandlerState::kOpenDoc);
}

auto ViewportReaderHandler::Int64(int64_t i) -> bool {
  return InvalidState();
}

auto ViewportReaderHandler::Uint64(uint64_t u) -> bool {
  return InvalidState();
}

auto ViewportReaderHandler::Double(double d) -> bool {
  return InvalidState();
}
}  // namespace prt::json