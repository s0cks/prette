#include "prette/window_event.h"

#include <lua.h>

#include "prette/to_string.h"

namespace prt {
void WindowEvent::ToTable(lua_State* L) const {
  ASSERT(L);
  Event::ToTable(L);
}

auto WindowCreatedEvent::ToString() const -> std::string {
  return ToStringHelper<WindowCreatedEvent>{};
}

auto WindowFramebufferSizeEvent::ToString() const -> std::string {
  ToStringHelper<WindowFramebufferSizeEvent> helper{};
  helper.AddFieldRef("width", GetWidth());
  helper.AddFieldRef("height", GetHeight());
  return helper;
}

auto WindowOpenedEvent::ToString() const -> std::string {
  return ToStringHelper<WindowOpenedEvent>{};
}

auto WindowClosedEvent::ToString() const -> std::string {
  return ToStringHelper<WindowClosedEvent>{};
}

auto WindowPosEvent::ToString() const -> std::string {
  ToStringHelper<WindowPosEvent> helper;
  helper.AddFieldRef("pos", GetPos());
  return helper;
}

auto WindowSizeEvent::ToString() const -> std::string {
  ToStringHelper<WindowSizeEvent> helper;
  helper.AddFieldRef("size", GetSize());
  return helper;
}

void WindowFocusEvent::ToTable(lua_State* L) const {
  WindowEvent::ToTable(L);
  lua_pushboolean(L, IsFocused());
  lua_setfield(L, -2, "focused");
}

auto WindowFocusEvent::ToString() const -> std::string {
  ToStringHelper<WindowFocusEvent> helper;
  helper.AddFieldRef("focused", IsFocused());
  return helper;
}

auto WindowIconifyEvent::ToString() const -> std::string {
  ToStringHelper<WindowIconifyEvent> helper;
  helper.AddFieldRef("iconified", IsIconified());
  return helper;
}

auto WindowRefreshEvent::ToString() const -> std::string {
  return ToStringHelper<WindowRefreshEvent>{};
}

auto WindowMaximizeEvent::ToString() const -> std::string {
  ToStringHelper<WindowMaximizeEvent> helper;
  helper.AddFieldRef("maximized", IsMaximized());
  return helper;
}

auto WindowContentScaleEvent::ToString() const -> std::string {
  ToStringHelper<WindowContentScaleEvent> helper;
  helper.AddFieldRef("content_scale", GetScale());
  return helper;
}
}  // namespace prt