#include "prette/render_pass_json.h"

#include <fmt/format.h>
#include <string>

#include "prette/json.h"
#include "prette/platform.h"
#include "prette/render_pass.h"

namespace prt::json {
auto RenderPassHandler::Key(const char* str, SizeType length, bool copy) -> bool {
  return InvalidState();
}

auto RenderPassHandler::String(const char* str, SizeType length, bool copy) -> bool {
  if (IsEmpty()) {
    std::string name(str, length);
    pass_ = vk::FindRenderPass(name);
    if (!pass_)
      return Error(fmt::format("failed to find RenderPass named `{0:s}`", name));
    return TransitionTo(RenderPassHandlerState::kClosedDoc);
  }
  return InvalidState();
}

auto RenderPassHandler::StartObject() -> bool {
  return InvalidState();
}

auto RenderPassHandler::EndObject(SizeType memberCount) -> bool {
  return InvalidState();
}

auto RenderPassHandler::StartArray() -> bool {
  return InvalidState();
}

auto RenderPassHandler::EndArray(SizeType elementCount) -> bool {
  return InvalidState();
}

auto RenderPassHandler::Null() -> bool {
  return InvalidState();
}

auto RenderPassHandler::Bool(bool b) -> bool {
  return InvalidState();
}

auto RenderPassHandler::Int(int i) -> bool {
  return InvalidState();
}

auto RenderPassHandler::Uint(unsigned u) -> bool {
  return InvalidState();
}

auto RenderPassHandler::Int64(int64_t i) -> bool {
  return InvalidState();
}

auto RenderPassHandler::Uint64(uint64_t u) -> bool {
  return InvalidState();
}

auto RenderPassHandler::Double(double d) -> bool {
  return InvalidState();
}
}  // namespace prt::json