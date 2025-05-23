#ifndef PRT_MERKLE_H
#define PRT_MERKLE_H

#include <algorithm>
#include <ostream>
#include <string>
#include <vector>

#include "prette/common.h"
#include "prette/sha256.h"
#include "prette/uint256.h"

// TODO: cleanup memory usage
namespace prt::merkle {
class Node;
class NodeVisitor {
 protected:
  NodeVisitor() = default;

 public:
  virtual ~NodeVisitor() = default;
  virtual auto Visit(Node* node) -> bool = 0;
};

class Node {
  DEFINE_DEFAULT_COPYABLE_TYPE(Node);

 protected:
  Node* parent_ = nullptr;
  Node* lchild_ = nullptr;
  Node* rchild_ = nullptr;
  uint256 hash_{};

  static inline auto ConcatHashes(Node* lhs, Node* rhs) -> uint256 {
    return sha256::Concat(lhs->GetHash(), rhs->GetHash());
  }

 public:
  Node() = default;
  explicit Node(const uint256& hash) :
    hash_(hash) {}
  ~Node() {
    if (lchild_)
      delete lchild_;
    if (rchild_)
      delete rchild_;
  }

  auto ToString() const -> std::string;

  auto GetParent() const -> Node* {
    return parent_;
  }

  auto HasParent() const -> bool {
    return parent_ != nullptr;
  }

  void SetParent(Node* node) {
    parent_ = node;
  }

  auto GetLeft() const -> Node* {
    return lchild_;
  }

  auto HasLeft() const -> bool {
    return lchild_ != nullptr;
  }

  void SetLeft(Node* node) {
    lchild_ = node;
  }

  auto GetRight() const -> Node* {
    return rchild_;
  }

  auto HasRight() const -> bool {
    return rchild_ != nullptr;
  }

  void SetRight(Node* node) {
    rchild_ = node;
  }

  auto GetHash() const -> const uint256& {
    return hash_;
  }

  auto IsLeaf() const -> bool {
    return !HasLeft() && !HasRight();
  }

  auto GetLeaves() const -> int {
    return IsLeaf() ? 1 : GetLeft()->GetLeaves() + GetRight()->GetLeaves();
  }

  auto CanVerifyHash() const -> bool {
    return HasLeft() && HasRight();
  }

  friend auto operator<<(std::ostream& stream, const Node& rhs) -> std::ostream& {
    return stream << rhs.ToString();
  }

  auto operator==(const Node& rhs) const -> bool {
    return GetHash() == rhs.GetHash();
  }

  auto operator!=(const Node& rhs) const -> bool {
    return GetHash() != rhs.GetHash();
  }

 public:
  static inline auto New(const uint256& hash) -> Node* {
    return new Node(hash);
  }

  static inline auto Concat(Node* left, Node* right) -> Node* {
    ASSERT(left);
    ASSERT(right);
    const auto node = New(ConcatHashes(left, right));
    ASSERT(node);
    node->SetLeft(left);
    node->SetRight(right);
    return node;
  }

  static inline auto Copy(Node* node) -> Node* {
    ASSERT(node);
    return New(node->GetHash());
  }
};

using NodeList = std::vector<Node*>;

class Tree {
 protected:
  Node* root_ = nullptr;
  std::vector<Node*> leaves_{};

  inline void SetRoot(Node* node) {
    ASSERT(node);
    root_ = node;
  }

  static auto ComputeRoot(const NodeList& nodes) -> Node*;

 public:
  Tree() = default;
  explicit Tree(const NodeList& leaves);
  explicit Tree(const std::vector<uint256>& leaves);
  Tree(const Tree& rhs) = default;
  ~Tree() {
    if (root_)
      delete root_;
  }

  auto GetRoot() const -> Node* {
    return root_;
  }

  auto HasRoot() const -> bool {
    return root_ != nullptr;
  }

  auto IsEmpty() const -> bool {
    return !HasRoot() && leaves_.empty();
  }

  auto GetLeaves() const -> const NodeList& {
    return leaves_;
  }

  auto GetRootHash() const -> const uint256& {
    ASSERT(HasRoot());
    return GetRoot()->GetHash();
  }

  auto VisitRoot(NodeVisitor* vis) const -> bool {
    ASSERT(vis);
    return HasRoot() ? vis->Visit(GetRoot()) : false;
  }

  auto VisitLeaves(NodeVisitor* vis) const -> bool {
    ASSERT(vis);
    for (const auto& leaf : leaves_) {
      if (!vis->Visit(leaf))
        return false;
    }
    return true;
  }

  auto Accept(NodeVisitor* vis) const -> bool {
    ASSERT(vis);
    return VisitRoot(vis) && VisitLeaves(vis);
  }

  auto operator=(const Tree& rhs) -> Tree& {
    root_ = rhs.root_;
    leaves_.clear();
    std::ranges::for_each(rhs.leaves_, [this](Node* node) {
      leaves_.push_back(node);
    });
    return *this;
  }
};
}  // namespace prt::merkle

#endif  // PRT_MERKLE_H
