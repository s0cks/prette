#ifndef PRT_TREE_H
#define PRT_TREE_H

#include "prette/common.h"

namespace prt::tree {
template <class N>
class Node {
 protected:
  N* parent_;

  explicit Node(N* parent = nullptr) :
    parent_(parent) {}

 public:
  virtual ~Node() = default;

  virtual auto GetParent() const -> N* {
    return parent_;
  }

  virtual void SetParent(N* node) {
    ASSERT(node);
    parent_ = node;
  }

  virtual auto GetNumberOfChildren() const -> uword {
    return 0;
  }

  virtual auto GetChildAt(const uword idx) const -> N* {
    return nullptr;
  }

  virtual void SetChildAt(const uword idx, N* node) {
    // do nothing
  }

  virtual void AddChild(N* node) {
    // do nothing
  }

  inline auto HasChildren() const -> bool {
    return GetNumberOfChildren() > 0;
  }

  inline auto IsLeaf() const -> bool {
    return GetNumberOfChildren() == 0;
  }

  inline auto IsRoot() const -> bool {
    return GetParent() == nullptr;
  }
};
}  // namespace prt::tree

#endif  // PRT_TREE_H