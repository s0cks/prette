#include "prette/mouse/mouse_event.h"

#include <string>

#include "prette/glm.h"
#include "prette/mouse/mouse.h"
#include "prette/to_string.h"

namespace prt {
auto MouseCreatedEvent::ToString() const -> std::string {
  ToStringHelper<MouseCreatedEvent> helper{};
  return helper;
}

auto MouseMotionEvent::IsDown() const -> bool {
  const auto delta = GetDirection();
  const auto& settings = GetMouseSettings();
  return settings.IsInverted() ? (delta.y > 0.0f) : (delta.y < 0.0f);
}

auto MouseMotionEvent::IsUp() const -> bool {
  const auto delta = GetDirection();
  const auto& settings = GetMouseSettings();
  return settings.IsInverted() ? (delta.y < 0.0f) : (delta.y > 0.0f);
}

auto MouseMotionEvent::ToString() const -> std::string {
  ToStringHelper<MouseMotionEvent> helper{};
  helper.AddFieldPtr("mouse", GetMouse());
  helper.AddField("direction", glm::to_string(GetDirection()));
  return helper;
}

auto MouseButtonState::ToString() const -> std::string {
  ToStringHelper<MouseButtonState> helper{};
  helper.AddFieldRef("button", GetButton());
  helper.AddFieldRef("action", GetAction());
  helper.AddFieldRef("mods", GetMods());
  return helper;
}

auto MouseButtonStateEvent::ToString() const -> std::string {
  ToStringHelper<MouseButtonStateEvent> helper{};
  helper.AddFieldRef("state", GetState());
  return helper;
}

auto ScrollEvent::ToString() const -> std::string {
  ToStringHelper<ScrollEvent> helper{};
  helper.AddField("delta", glm::to_string(GetDelta()));
  return helper;
}

auto ScrollEvent::IsScrollUp() const -> bool {
  const auto dY = GetDeltaY();
  const auto& settings = GetMouseSettings();
  return settings.IsInverted() ? (dY > 0.0f) : (dY < 0.0f);
}

auto ScrollEvent::IsScrollDown() const -> bool {
  const auto dY = GetDeltaY();
  const auto& settings = GetMouseSettings();
  return settings.IsInverted() ? (dY < 0.0f) : (dY > 0.0f);
}

auto DragStartEvent::ToString() const -> std::string {
  ToStringHelper<DragStartEvent> helper{};
  return helper;
}

auto DragFinishedEvent::IsDragDown() const -> bool {
  const auto delta = GetDelta();
  const auto& settings = GetMouseSettings();
  return settings.IsInverted() ? (delta.y > 0.0f) : (delta.y < 0.0f);
}

auto DragFinishedEvent::IsDragUp() const -> bool {
  const auto delta = GetDelta();
  const auto& settings = GetMouseSettings();
  return settings.IsInverted() ? (delta.y < 0.0f) : (delta.y > 0.0f);
}

auto DragFinishedEvent::ToString() const -> std::string {
  ToStringHelper<DragFinishedEvent> helper{};
  return helper;
}

auto MouseDestroyedEvent::ToString() const -> std::string {
  ToStringHelper<MouseDestroyedEvent> helper{};
  return helper;
}
}  // namespace prt