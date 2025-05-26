#include "prette/render_pass/render_pass_attachments_json.h"

#include <cstdint>

#include "prette/json.h"

namespace prt::json {
auto RenderPassAttachmentsHandler::Key(const char* str, SizeType length, bool copy) -> bool {
  return InvalidState();
}

auto RenderPassAttachmentsHandler::String(const char* str, SizeType length, bool copy) -> bool {
  return InvalidState();
}

auto RenderPassAttachmentsHandler::StartObject() -> bool {
  return InvalidState();
}

auto RenderPassAttachmentsHandler::EndObject(SizeType memberCount) -> bool {
  return InvalidState();
}

auto RenderPassAttachmentsHandler::StartArray() -> bool {
  return InvalidState();
}

auto RenderPassAttachmentsHandler::EndArray(SizeType elementCount) -> bool {
  return InvalidState();
}

auto RenderPassAttachmentsHandler::Null() -> bool {
  return InvalidState();
}

auto RenderPassAttachmentsHandler::Bool(bool b) -> bool {
  return InvalidState();
}

auto RenderPassAttachmentsHandler::Int(int i) -> bool {
  return InvalidState();
}

auto RenderPassAttachmentsHandler::Uint(unsigned u) -> bool {
  return InvalidState();
}

auto RenderPassAttachmentsHandler::Int64(int64_t i) -> bool {
  return InvalidState();
}

auto RenderPassAttachmentsHandler::Uint64(uint64_t u) -> bool {
  return InvalidState();
}

auto RenderPassAttachmentsHandler::Double(double d) -> bool {
  return InvalidState();
}
}  // namespace prt::json