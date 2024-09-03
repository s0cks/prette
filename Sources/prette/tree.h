#ifndef PRT_TREE_H
#define PRT_TREE_H

#include "prette/common.h"

namespace prt::tree {
  template<class N>
  class Node {
  protected:
    N* parent_;

    explicit Node(N* parent = nullptr):
      parent_(parent) {
    }
  public:
    virtual ~Node() = default;

    virtual N* GetParent() const {
      return parent_;
    }

    virtual void SetParent(N* node) {
      PRT_ASSERT(node);
      parent_ = node;
    }

    virtual uword GetNumberOfChildren() const {
      return 0;
    }

    virtual N* GetChildAt(const uword idx) const {
      return nullptr;
    }

    virtual void SetChildAt(const uword idx, N* node) {
      // do nothing
    }

    virtual void AddChild(N* node) {
      // do nothing
    }

    inline bool HasChildren() const {
      return GetNumberOfChildren() > 0;
    }

    inline bool IsLeaf() const {
      return GetNumberOfChildren() == 0;
    }

    inline bool IsRoot() const {
      return GetParent() == nullptr;
    }
  };
}

#endif //PRT_TREE_H