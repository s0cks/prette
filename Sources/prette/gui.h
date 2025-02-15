#ifndef PRT_GUI_H
#define PRT_GUI_H

#include <fmt/format.h>
#include <imgui_freetype.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <implot.h>

#include "prette/event.h"
#include "prette/gfx.h"

namespace prt {
#define FOR_EACH_GUI_EVENT(V) \
  V(GuiInit)                  \
  V(GuiOpened)                \
  V(GuiRendered)              \
  V(GuiUpdated)               \
  V(GuiClosed)

class Gui;
class GuiEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
FOR_EACH_GUI_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

class GuiEvent : public Event {
 private:
  Gui* gui_;

 protected:
  explicit GuiEvent(Gui* gui) :
    Event(),
    gui_(gui) {
    ASSERT(gui_);
  }

 public:
  ~GuiEvent() override = default;

  auto GetGui() const -> Gui* {
    return gui_;
  }

  DEFINE_EVENT_PROTOTYPE(Gui, FOR_EACH_GUI_EVENT);
};

class GuiInitEvent : public GuiEvent {
 public:
  explicit GuiInitEvent(Gui* gui) :
    GuiEvent(gui) {}
  ~GuiInitEvent() override = default;
  DECLARE_EVENT_TYPE(GuiEvent, GuiInit);
};

class GuiOpenedEvent : public GuiEvent {
 public:
  explicit GuiOpenedEvent(Gui* gui) :
    GuiEvent(gui) {}
  ~GuiOpenedEvent() override = default;
  DECLARE_EVENT_TYPE(GuiEvent, GuiOpened);
};

class GuiRenderedEvent : public GuiEvent {
 public:
  explicit GuiRenderedEvent(Gui* gui) :
    GuiEvent(gui) {}
  ~GuiRenderedEvent() override = default;
  DECLARE_EVENT_TYPE(GuiEvent, GuiRendered);
};

class GuiUpdatedEvent : public GuiEvent {
 public:
  explicit GuiUpdatedEvent(Gui* gui) :
    GuiEvent(gui) {}
  ~GuiUpdatedEvent() override = default;
  DECLARE_EVENT_TYPE(GuiEvent, GuiUpdated);
};

class GuiClosedEvent : public GuiEvent {
 public:
  explicit GuiClosedEvent(Gui* gui) :
    GuiEvent(gui) {}
  ~GuiClosedEvent() override = default;
  DECLARE_EVENT_TYPE(GuiEvent, GuiClosed);
};

DEFINE_EVENT_SUBJECT(Gui);
DEFINE_EVENT_OBSERVABLE(Gui);
FOR_EACH_GUI_EVENT(DEFINE_EVENT_OBSERVABLE);

class Gui {
 private:
  std::string name_;
  rx::subscription on_tick_{};

 protected:
  explicit Gui(std::string name);

  void LoadLuaScript(const std::string& name) {
    const auto lua = LuaState::Get();
    ASSERT(lua);
    lua->ExecuteScript(name);
  }

  template <typename... Args>
  void InvokeLuaCallback(const std::string& name, Args... args) {
    const auto lua = LuaState::Get();
    ASSERT(lua);
    lua->ExecuteGlobalFunction(name_, name);
  }

 public:
  virtual ~Gui();
  virtual auto GetGuiName() const -> const char* = 0;

 public:  // TODO: reduce visibility
  virtual void Update() = 0;
  virtual void Render() = 0;
};

namespace gui {
struct PushConstBlock {
  glm::vec2 scale{};
  glm::vec2 translate{};
};

void Init();
void Shutdown();
auto Update(const glm::u32vec2& size) -> bool;
}  // namespace gui
}  // namespace prt

#include "prette/gui_renderer.h"

#endif  // PRT_GUI_H
