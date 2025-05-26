#ifndef PRT_GUI_EVENT_H
#define PRT_GUI_EVENT_H

#include "prette/assertions.h"
#include "prette/event.h"

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

  DEFINE_EVENT_PROTOTYPE_TYPE(Gui, FOR_EACH_GUI_EVENT);
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
}  // namespace prt

#endif  // PRT_GUI_EVENT_H
