#include "prette/gui/gui_window.h"

#include "prette/gui/gui_tree.h"
#include "prette/window/window.h"

namespace prt::gui {
  Window::Window():
    ContainerComponent() {
    const auto win = window::GetAppWindow();
    PRT_ASSERT(win);
    const auto frame = win->GetFrame();
    PRT_ASSERT(frame);
    frame->AddChild(this);
  }

  Window::~Window() {
    const auto frame = (Frame*) GetParent();
    //TODO: remove child from frame
  }
}