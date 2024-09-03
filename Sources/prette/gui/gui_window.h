#ifndef PRT_GUI_WINDOW_H
#define PRT_GUI_WINDOW_H

#include "prette/color.h"
#include "prette/gui/gui_events.h"
#include "prette/gui/gui_component.h"

namespace prt {
  namespace render {
    class ComponentRenderer;
  }

  namespace gui {
    class Window : public ContainerComponent {
      friend class prt::render::ComponentRenderer;
    protected:
      Color bg_;

      Window();

      bool Accept(ComponentVisitor* vis) override {
        return vis->VisitWindow(this);
      }

      void SetBackground(const Color& color) {
        bg_ = color;
      }
    public:
      ~Window();
      
      const Color& GetBackground() const {
        return bg_;
      }

      inline rx::observable<WindowOpenedEvent*> OnOpened() const {
        return OnEvent()
          .filter(WindowOpenedEvent::Filter)
          .map(WindowOpenedEvent::Cast);
      }

      inline rx::observable<WindowClosedEvent*> OnClosed() const {
        return OnEvent()
          .filter(WindowClosedEvent::Filter)
          .map(WindowClosedEvent::Cast);
      }
    public:
      static Window* New() {
        return new Window();
      }
    };
  }
}

#endif //PRT_GUI_WINDOW_H