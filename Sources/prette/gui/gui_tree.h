#ifndef PRT_GUI_TREE_H
#define PRT_GUI_TREE_H

#include "prette/gui/gui_component.h"

namespace prt::gui {
  class Tree {
    friend class Frame;
    DEFINE_NON_INSTANTIABLE_TYPE(Tree);
  private:
    static void AddRoot(Component* root);
    static void RemoveRoot(const int32_t idx);
    static void RemoveRoot(Component* root);

    static void Publish(RootComponentEvent* event);

    template<class E, typename... Args>
    static void Publish(Args... args) {
      E event(args...);
      return Publish((RootComponentEvent*) &event);
    }
  public:
    static int32_t GetNumberOfRoots();
    static Component* GetRoot(const int32_t idx);
    static rx::observable<Component*> GetAllRoots();
    static rx::observable<RootComponentEvent*> OnEvent();
    static bool VisitAllRoots(ComponentVisitor* vis);

    static inline bool
    HasRoots() {
      return GetNumberOfRoots() > 0;
    }

    static inline bool
    IsEmpty() {
      return GetNumberOfRoots() == 0;
    }

    static inline rx::observable<RootComponentRegisteredEvent*>
    OnRootRegistered() {
      return OnEvent()
        .filter(RootComponentRegisteredEvent::Filter)
        .map(RootComponentRegisteredEvent::Cast);
    }

    static inline rx::observable<RootComponentRemovedEvent*>
    OnRootRemoved() {
      return OnEvent()
        .filter(RootComponentRemovedEvent::Filter)
        .map(RootComponentRemovedEvent::Cast);
    }
  };
}

#endif //PRT_GUI_TREE_H