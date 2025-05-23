#include "prette/renderer_event.h"

#include <string>

#include "prette/renderer_state.h"
#include "prette/to_string.h"

namespace prt {
auto RendererStateEvent::GetState() const -> RendererState {
  return state_;
}

auto RendererStateEvent::ToString() const -> std::string {
  ToStringHelper<RendererStateEvent> helper{};
  return helper;
}

auto PreFrameEvent::ToString() const -> std::string {
  ToStringHelper<PreFrameEvent> helper{};
  return helper;
}

auto PostFrameEvent::ToString() const -> std::string {
  ToStringHelper<PostFrameEvent> helper{};
  return helper;
}
}  // namespace prt