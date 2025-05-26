#include "prette/mouse/mouse.h"

#include <gflags/gflags.h>
#include <utility>

#include "prette/assertions.h"
#include "prette/camera.h"
#include "prette/common.h"
#include "prette/glm.h"
#include "prette/gui/gui_viewport.h"
#include "prette/mouse/mouse_event.h"
#include "prette/to_string.h"
#include "prette/window/window.h"

namespace prt {
static MouseEventSubject events_{};
static Mouse* mouse_ = nullptr;

auto Mouse::Get() -> Mouse* {
  ASSERT(IsInitialized());
  return mouse_;
}

void Mouse::PublishEvent(MouseEvent* event) {
  ASSERT(event);
  const auto& subscriber = events_.get_subscriber();
  return subscriber.on_next(event);
}

auto OnMouseEvent() -> MouseEventObservable {
  return events_.get_observable();
}

auto Mouse::IsInitialized() -> bool {
  return mouse_ != nullptr;
}

static inline auto SetMouse(Mouse* rhs) -> Mouse* {
  ASSERT(rhs);
  mouse_ = rhs;
  return rhs;
}

auto Mouse::GetNormalizedDeviceCoords() const -> glm::vec2 {
  if (GuiViewport::IsInitialized())
    return ToNormalizedDeviceCoords(GetPos(), GuiViewport::Get()->GetSize());
  return ToNormalizedDeviceCoords(GetPos(), GetAppWindow()->GetSize());
}

void Mouse::StartDragging(const int button, const glm::vec2 pos, const int mods) {
  dragging_ = true;
  drag_state_ = {
      .button = button,
      .mods = mods,
      .start = pos,
      .finish = glm::vec2(),
  };
  Publish<DragStartEvent>(drag_state_);
}

void Mouse::StopDragging(const glm::vec2 pos) {
  dragging_ = false;
  drag_state_.finish = std::move(pos);
  Publish<DragFinishedEvent>(drag_state_);
}

auto Mouse::GetWorldPos() const -> glm::vec2 {
  const auto camera = GetCamera();
  const auto world_pos = camera->Unproject(GetNormalizedDeviceCoords());
  return {round(world_pos.x), round(world_pos.y)};
}

auto Mouse::New(Window* window) -> Mouse* {
  const auto mouse = new Mouse(window);
  ASSERT(mouse);
  Publish<MouseCreatedEvent>(mouse);
  return mouse;
}

auto Mouse::Init(Window* window) -> Mouse* {
  ASSERT(window);
  ASSERT(!IsInitialized());
  const auto mouse = SetMouse(Mouse::New(window));
  ASSERT(mouse);
  window->SetMouse(mouse);
  return mouse;
}
}  // namespace prt