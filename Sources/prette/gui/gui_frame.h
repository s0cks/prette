#ifndef PRT_GUI_FRAME_H
#define PRT_GUI_FRAME_H

#include "prette/rx.h"
#include "prette/gui/gui_component.h"

namespace prt {
  namespace window {
    class Window;
  }

  namespace gui {
    class Frame : public ContainerComponent {
      friend class prt::window::Window;
    protected:
      // only constructable by window::Window
      explicit Frame(const Rectangle& bounds = {});
      void Render(Context* ctx) override;
      bool Accept(ComponentVisitor* vis) override;
    public:
      ~Frame() override;
    };
  }
}

#endif //PRT_GUI_FRAME_H