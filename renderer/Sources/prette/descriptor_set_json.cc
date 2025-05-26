#include "prette/descriptor_set_json.h"

#include "prette/platform.h"

namespace prt::json {
auto DescriptorSetHandler::Null() -> bool {
  return InvalidState();
}

auto DescriptorSetHandler::Bool(bool b) -> bool {
  return InvalidState();
}

auto DescriptorSetHandler::Int(int i) -> bool {
  return InvalidState();
}

auto DescriptorSetHandler::Uint(unsigned u) -> bool {
  return InvalidState();
}

auto DescriptorSetHandler::Int64(int64_t i) -> bool {
  return InvalidState();
}

auto DescriptorSetHandler::Uint64(uint64_t u) -> bool {
  return InvalidState();
}

auto DescriptorSetHandler::Double(double d) -> bool {
  return InvalidState();
}

auto DescriptorSetHandler::Key(const char* str, SizeType length, bool copy) -> bool {
  return InvalidState();
}

auto DescriptorSetHandler::String(const char* str, SizeType length, bool copy) -> bool {
  return InvalidState();
}

auto DescriptorSetHandler::StartObject() -> bool {
  return InvalidState();
}

auto DescriptorSetHandler::EndObject(SizeType memberCount) -> bool {
  return InvalidState();
}

auto DescriptorSetHandler::StartArray() -> bool {
  return InvalidState();
}

auto DescriptorSetHandler::EndArray(SizeType elementCount) -> bool {
  return InvalidState();
}
}  // namespace prt::json