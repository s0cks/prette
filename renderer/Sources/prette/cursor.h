#ifndef PRT_CURSOR_H
#define PRT_CURSOR_H

#include <GLFW/glfw3.h>
#include <absl/container/flat_hash_map.h>
#include <concepts>
#include <cstdint>
#include <string>

namespace prt {
template <class C>
concept CursorDesc = requires {
  { C::kCursorName } -> std::convertible_to<const char*>;
  { C::kTotalNumberOfBytes } -> std::convertible_to<uint64_t>;
  { C::kBytes };
};

auto NewCursor(const uint8_t* bytes, const int width = 16, const int height = 16) -> GLFWcursor*;

template <CursorDesc C>
inline auto NewCursor(const int width = 16, const int height = 16) -> GLFWcursor* {
  return NewCursor(&C::kBytes[0], width, height);
}

using CursorMap = absl::flat_hash_map<std::string, GLFWcursor*>;
}  // namespace prt

#endif  // PRT_CURSOR_H
