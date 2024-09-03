#include "prette/gui/gui_tree.h"

#include <vector>
#include "prette/thread_local.h"

namespace prt::gui {
  static rx::subject<RootComponentEvent*> events_;
  static std::vector<Component*> roots_;

  int32_t Tree::GetNumberOfRoots() {
    return roots_.size();
  }

  Component* Tree::GetRoot(const int32_t idx) {
    PRT_ASSERT(idx >= 0 && idx <= GetNumberOfRoots());
    return roots_[idx];
  }

  void Tree::AddRoot(Component* root) {
    PRT_ASSERT(root);
    roots_.push_back(root);
    Publish<RootComponentRegisteredEvent>(root);
  }

  void Tree::RemoveRoot(const int32_t idx) {
    PRT_ASSERT(idx >= 0 && idx <= GetNumberOfRoots());
    NOT_IMPLEMENTED(ERROR); //TODO: implement
  }

  void Tree::RemoveRoot(Component* root) {
    PRT_ASSERT(root);
    NOT_IMPLEMENTED(ERROR);//TODO: implement
  }

  void Tree::Publish(RootComponentEvent* event) {
    PRT_ASSERT(event);
    const auto& subscriber = events_.get_subscriber();
    subscriber.on_next(event);
  }

  rx::observable<Component*> Tree::GetAllRoots() {
    return rx::observable<>::iterate(roots_);
  }

  rx::observable<RootComponentEvent*> Tree::OnEvent() {
    return events_.get_observable();
  }

  bool Tree::VisitAllRoots(ComponentVisitor* vis) {
    PRT_ASSERT(vis);
    for(const auto& root : roots_) {
      if(!root->Accept(vis))
        return false;
    }
    return true;
  }
}