#include "prette/gui/gui_component.h"

#include "prette/gui/gui_frame.h"
#include "prette/gui/gui_window.h"

namespace prt::gui {
#define DEFINE_VISIT_COMPONENT(Name)                \
  bool ComponentRenderer::Visit##Name(Name* c) {    \
    PRT_ASSERT(c);                                  \
    RenderComponent(c);                             \
    return c->VisitChildren(this);                  \
  }
  FOR_EACH_GUI_COMPONENT_TYPE(DEFINE_VISIT_COMPONENT)
#undef DEFINE_VISIT_COMPONENT
}