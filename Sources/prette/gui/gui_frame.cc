#include "prette/gui/gui_frame.h"
#include "prette/gui/gui_tree.h"

#include "prette/mouse/mouse.h"

namespace prt::gui {
  Frame::Frame(const Rectangle& bounds):
    ContainerComponent() {
    SetBounds(bounds);
    Tree::AddRoot(this);
  }

  Frame::~Frame() {
    Tree::RemoveRoot(this);
  }

  void Frame::Render(Context* ctx) {
    // PRT_ASSERT(ctx);
    // ctx->SetColor(kGreen);
    // const auto transform = glm::mat4(1.0f);
    // ctx->DrawRect(transform, GetBounds());
  }

  bool Frame::Accept(ComponentVisitor* vis) {
    PRT_ASSERT(vis);
    return vis->VisitFrame(this);
  }
}