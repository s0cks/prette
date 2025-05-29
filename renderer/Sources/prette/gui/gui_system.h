#ifndef PRT_GUI_SYSTEM_H
#define PRT_GUI_SYSTEM_H

#include <vector>

#include "prette/gui/gui_render_pass.h"
#include "prette/relaxed_atomic.h"
#include "prette/rx.h"

namespace prt {
namespace vk {
class RenderPass;
}

class Gui;
class GuiRenderPass;
class GuiSystem {
 private:
  RelaxedAtomic<bool> layouts_changed_ = true;
  rx::subscription on_pre_frame_{};
  rx::subscription on_post_init_{};
  rx::subscription on_destroying_device_{};
  rx::subscription on_swap_created_{};
  std::vector<Gui*> guis_{};
  GuiRenderPass* pass_ = nullptr;

  void UpdateImGui();
  void Shutdown();
  void UpdateLayout();

  inline auto HaveLayoutsChanged() const -> bool {
    return (bool)layouts_changed_;
  }

  inline void SetLayoutsChanged(const bool rhs = true) {
    layouts_changed_ = rhs;
  }

  inline void ClearLayoutsChanged() {
    return SetLayoutsChanged(false);
  }

 public:
  GuiSystem();
  ~GuiSystem();

  auto GetGuiPass() const -> GuiRenderPass* {
    return pass_;
  }

 public:
  static void InitSystem();
  static auto GetSystem() -> GuiSystem*;
  static auto IsSystemInitialized() -> bool;
};
}  // namespace prt

#endif  // PRT_GUI_SYSTEM_H
