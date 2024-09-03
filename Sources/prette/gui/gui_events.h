#ifndef PRT_GUI_EVENTS_H
#define PRT_GUI_EVENTS_H

#include "prette/event.h"
#include "prette/shape.h"
#include "prette/common.h"
#include "prette/mouse/mouse_button.h"

namespace prt::gui {
#define FOR_EACH_GUI_EVENT(V)       \
  V(RootComponentRegistered)        \
  V(RootComponentRemoved)           \
  V(WindowOpened)                   \
  V(WindowClosed)                   \
  V(MouseEnter)                     \
  V(MouseExit)                      \
  V(MouseClick)

  class GuiEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
  FOR_EACH_GUI_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

  class GuiEvent : public Event {
  protected:
    GuiEvent() = default;
  public:
    ~GuiEvent() override = default;

    virtual bool IsWindowEvent() const {
      return false;
    }
    
    virtual bool IsMouseEvent() const {
      return false;
    }
    DEFINE_EVENT_PROTOTYPE(FOR_EACH_GUI_EVENT);
  };

#define DECLARE_GUI_EVENT(Name)                                               \
  DECLARE_EVENT_TYPE(GuiEvent, Name)
  
  class Component;
  class RootComponentEvent : public GuiEvent {
  protected:
    const Component* component_;

    explicit RootComponentEvent(const Component* component):
      GuiEvent(),
      component_(component) {
      PRT_ASSERT(component);
    }
  public:
    ~RootComponentEvent() override = default;

    const Component* GetComponent() const {
      return component_;
    }
  };
  
  class RootComponentRegisteredEvent : public RootComponentEvent {
  public:
    explicit RootComponentRegisteredEvent(const Component* component):
      RootComponentEvent(component) {
    }
    ~RootComponentRegisteredEvent() override = default;
    DECLARE_GUI_EVENT(RootComponentRegistered);
  };

  class RootComponentRemovedEvent : public RootComponentEvent {
  public:
    explicit RootComponentRemovedEvent(const Component* component):
      RootComponentEvent(component) {
    }
    ~RootComponentRemovedEvent() override = default;
    DECLARE_GUI_EVENT(RootComponentRemoved);
  };

  class Window;
  class WindowEvent : public GuiEvent {
  protected:
    const Window* window_;

    explicit WindowEvent(const Window* window):
      GuiEvent(),
      window_(window) {
    }
  public:
    ~WindowEvent() override = default;

    bool IsWindowEvent() const override {
      return true;
    }

    const Window* GetWindow() const {
      return window_;
    }
  };

  class WindowOpenedEvent : public WindowEvent {
  public:
    explicit WindowOpenedEvent(const Window* window):
      WindowEvent(window) {
    }
    ~WindowOpenedEvent() override = default;
    DECLARE_GUI_EVENT(WindowOpened);
  };

  class WindowClosedEvent : public WindowEvent {
  public:
    explicit WindowClosedEvent(const Window* window):
      WindowEvent(window) {
    }
    ~WindowClosedEvent() override = default;
    DECLARE_GUI_EVENT(WindowClosed);
  };

  class MouseEvent : public GuiEvent {
  protected:
    Point pos_;

    explicit MouseEvent(const Point& pos):
      GuiEvent(),
      pos_(pos) {
    }
  public:
    ~MouseEvent() override = default;

    bool IsMouseEvent() const override {
      return true;
    }

    const Point& GetPos() const {
      return pos_;
    }
  };

  class MouseClickEvent : public MouseEvent {
  protected:
    mouse::MouseButton button_;
  public:
    MouseClickEvent(const Point& pos, const mouse::MouseButton button):
      MouseEvent(pos),
      button_(button) {
    }
    ~MouseClickEvent() override = default;

    mouse::MouseButton GetButton() const {
      return button_;
    }

    DECLARE_GUI_EVENT(MouseClick);
  };

  class MouseEnterEvent : public MouseEvent {
  public:
    explicit MouseEnterEvent(const Point& pos):
      MouseEvent(pos) {
    }
    ~MouseEnterEvent() override = default;
    DECLARE_GUI_EVENT(MouseEnter);
  };

  class MouseExitEvent : public MouseEvent {
  public:
    explicit MouseExitEvent(const Point& pos):
      MouseEvent(pos) {
    }
    ~MouseExitEvent() override = default;
    DECLARE_GUI_EVENT(MouseExit);
  };

  typedef rx::subject<GuiEvent*> GuiEventSubject;

  class GuiEventSource : public EventSource<GuiEvent> {
  protected:
    GuiEventSource() = default;
  public:
    ~GuiEventSource() override = default;

#define DEFINE_ON_EVENT(Name)                       \
    inline rx::observable<Name##Event*>             \
    On##Name() const {                              \
      return OnEvent()                              \
          .filter(Name##Event::Filter)              \
          .map(Name##Event::Cast);                  \
    }
    FOR_EACH_GUI_EVENT(DEFINE_ON_EVENT)
#undef DEFINE_ON_EVENT
  };
}

#endif //PRT_GUI_EVENTS_H