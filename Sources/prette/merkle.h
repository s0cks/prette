#ifndef PRT_MERKLE_H
#define PRT_MERKLE_H

#include <string>
#include <vector>
#include <ostream>

#include "prette/sha256.h"

//TODO: cleanup memory usage
namespace prt::merkle {
  class Node;
  class NodeVisitor {
  protected:
    NodeVisitor() = default;
  public:
    virtual ~NodeVisitor() = default;
    virtual bool Visit(Node* node) = 0;
  };

  class Node {
    DEFINE_DEFAULT_COPYABLE_TYPE(Node);
  protected:
    Node* parent_;
    Node* lchild_;
    Node* rchild_;
    uint256 hash_;

    static inline uint256
    ConcatHashes(Node* lhs, Node* rhs) {
      return sha256::Concat(lhs->GetHash(), rhs->GetHash());
    }
  public:
    Node() = default;
    explicit Node(const uint256& hash):
      parent_(nullptr),
      lchild_(nullptr),
      rchild_(nullptr),
      hash_(hash) {
    }
    ~Node() {
      if(lchild_)
        delete lchild_;
      if(rchild_)
        delete rchild_;
    }

    std::string ToString() const;

    Node* GetParent() const {
      return parent_;
    }

    bool HasParent() const {
      return parent_ != nullptr;
    }

    void SetParent(Node* node) {
      parent_ = node;
    }

    Node* GetLeft() const {
      return lchild_;
    }

    bool HasLeft() const {
      return lchild_ != nullptr;
    }

    void SetLeft(Node* node) {
      lchild_ = node;
    }

    Node* GetRight() const {
      return rchild_;
    }

    bool HasRight() const {
      return rchild_ != nullptr;
    }

    void SetRight(Node* node) {
      rchild_ = node;
    }

    const uint256& GetHash() const {
      return hash_;
    }

    bool IsLeaf() const {
      return !HasLeft() && !HasRight();
    }

    int GetLeaves() const {
      return IsLeaf() 
           ? 1
           : GetLeft()->GetLeaves() + GetRight()->GetLeaves();
    }

    bool CanVerifyHash() const {
      return HasLeft() && HasRight();
    }

    friend std::ostream& operator<<(std::ostream& stream, const Node& rhs) {
      return stream << rhs.ToString();
    }

    bool operator==(const Node& rhs) const {
      return GetHash() == rhs.GetHash();
    }
    
    bool operator!=(const Node& rhs) const {
      return GetHash() != rhs.GetHash();
    }
  public:
    static inline Node*
    New(const uint256& hash) {
      return new Node(hash);
    }

    static inline Node*
    Concat(Node* left, Node* right) {
      PRT_ASSERT(left);
      PRT_ASSERT(right);
      const auto node = New(ConcatHashes(left, right));
      PRT_ASSERT(node);
      node->SetLeft(left);
      node->SetRight(right);
      return node;
    }

    static inline Node*
    Copy(Node* node) {
      PRT_ASSERT(node);
      return New(node->GetHash());
    }
  };

  typedef std::vector<Node*> NodeList;

  class Tree {
  protected:
    Node* root_;
    std::vector<Node*> leaves_;

    inline void SetRoot(Node* node) {
      PRT_ASSERT(node);
      root_ = node;
    }

    static Node* ComputeRoot(const NodeList& nodes);
  public:
    Tree() = default;
    explicit Tree(const NodeList& leaves);
    explicit Tree(const std::vector<uint256>& leaves);
    Tree(const Tree& rhs):
      root_(rhs.root_),
      leaves_(rhs.leaves_) {
    }
    ~Tree() {
      if(root_)
        delete root_;
    }

    Node* GetRoot() const {
      return root_;
    }

    bool HasRoot() const {
      return root_ != nullptr;
    }

    bool IsEmpty() const {
      return !HasRoot()
          && leaves_.empty();
    }

    const NodeList& GetLeaves() const {
      return leaves_;
    }

    const uint256& GetRootHash() const {
      PRT_ASSERT(HasRoot());
      return GetRoot()->GetHash();
    }

    bool VisitRoot(NodeVisitor* vis) const {
      PRT_ASSERT(vis);
      return HasRoot()
           ? vis->Visit(GetRoot())
           : false;
    }

    bool VisitLeaves(NodeVisitor* vis) const {
      PRT_ASSERT(vis);
      for(const auto& leaf : leaves_) {
        if(!vis->Visit(leaf))
          return false;
      }
      return true;
    }

    bool Accept(NodeVisitor* vis) const {
      PRT_ASSERT(vis);
      return VisitRoot(vis) && VisitLeaves(vis);
    }

    Tree& operator=(const Tree& rhs) {
      root_ = rhs.root_;
      leaves_.clear();
      std::for_each(std::begin(rhs.leaves_), std::begin(rhs.leaves_), [this](Node* node) {
        leaves_.push_back(node);
      });
      return *this;
    }
  };
}

#endif //PRT_MERKLE_H