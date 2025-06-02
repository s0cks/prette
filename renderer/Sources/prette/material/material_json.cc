#include "prette/material/material_json.h"

#include <cstdint>
#include <rapidjson/rapidjson.h>

namespace prt::json {
auto MaterialReaderHandler::Null() -> bool {
  return InvalidState();
}

auto MaterialReaderHandler::Bool(bool b) -> bool {
  return InvalidState();
}

auto MaterialReaderHandler::Int(int i) -> bool {
  return InvalidState();
}

auto MaterialReaderHandler::Uint(unsigned u) -> bool {
  return InvalidState();
}

auto MaterialReaderHandler::Int64(int64_t i) -> bool {
  return InvalidState();
}

auto MaterialReaderHandler::Uint64(uint64_t u) -> bool {
  return InvalidState();
}

auto MaterialReaderHandler::Double(double d) -> bool {
  return InvalidState();
}

auto MaterialReaderHandler::Key(const char* str, SizeType length, bool copy) -> bool {
  return InvalidState();
}

auto MaterialReaderHandler::String(const char* str, SizeType length, bool copy) -> bool {
  return InvalidState();
}

auto MaterialReaderHandler::StartObject() -> bool {
  return InvalidState();
}

auto MaterialReaderHandler::EndObject(SizeType memberCount) -> bool {
  return InvalidState();
}

auto MaterialReaderHandler::StartArray() -> bool {
  return InvalidState();
}

auto MaterialReaderHandler::EndArray(SizeType elementCount) -> bool {
  return InvalidState();
}
}  // namespace prt::json