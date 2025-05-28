#include "prette/shader/shader_json.h"

#include <fmt/format.h>
#include <optional>
#include <string>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/json.h"
#include "prette/shader/shader.h"

namespace prt::json {
auto ShaderObjectHandler::OnParseCode(const fs::path p) -> bool {
  DLOG(INFO) << "shader code: " << p;
  return Continue();
}

auto ShaderObjectHandler::String(const char* str, SizeType length, bool copy) -> bool {
  const auto value = std::string(str, length);
  switch (GetState()) {
    case ShaderObjectHandlerState::kParsingCode: {
      const auto code_path = vk::ResolveShaderCodePath(value);
      if (!code_path)
        return Error(fmt::format("failed to find ShaderCode: {}", value));
      if (!OnParseCode((*code_path)))
        return TransitionTo(ShaderObjectHandlerState::kError);
      return TransitionTo(ShaderObjectHandlerState::kOpen);
    }
    default:
      return InvalidState();
  }
}

auto ShaderObjectHandler::EndObject(SizeType memberCount) -> bool {
  switch (GetState()) {
    case ShaderObjectHandlerState::kOpen:
      return TransitionTo(ShaderObjectHandlerState::kClosed);
    default:
      break;
  }
  return InvalidState();
}

auto ShaderObjectHandler::Null() -> bool {
  return InvalidState();
}

auto ShaderObjectHandler::Bool(bool b) -> bool {
  return InvalidState();
}

auto ShaderObjectHandler::Int(int i) -> bool {
  return InvalidState();
}

auto ShaderObjectHandler::Uint(unsigned u) -> bool {
  return InvalidState();
}

auto ShaderObjectHandler::Int64(int64_t i) -> bool {
  return InvalidState();
}

auto ShaderObjectHandler::Uint64(uint64_t u) -> bool {
  return InvalidState();
}

auto ShaderObjectHandler::Double(double d) -> bool {
  return InvalidState();
}

auto ShaderObjectHandler::StartArray() -> bool {
  return InvalidState();
}

auto ShaderObjectHandler::EndArray(SizeType elementCount) -> bool {
  return InvalidState();
}

auto ShaderObjectHandler::StartObject() -> bool {
  switch (GetState()) {
    case ShaderObjectHandlerState::kClosed:
      return TransitionTo(ShaderObjectHandlerState::kOpen);
    default:
      return InvalidState();
  }
}

#define FOR_EACH_SHADER_OBJECT_KEY(V) V("code", ShaderObjectHandlerState::kParsingCode)

auto ShaderObjectHandler::Key(const char* str, SizeType length, bool copy) -> bool {
  const auto key = std::string(str, length);
  ASSERT(IsOpen());
#define CHECK_KEY(Key, State)       \
  if (EqualsIgnoreCase((Key), key)) \
    return TransitionTo((State));
  FOR_EACH_SHADER_OBJECT_KEY(CHECK_KEY);
  return InvalidState();
#undef CHECK_KEY
}
#undef FOR_EACH_SHADER_OBJECT_KEY
}  // namespace prt::json