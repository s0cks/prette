#include "prette/merkle.h"

namespace prt::merkle {
  Node* Tree::ComputeRoot(const NodeList& nodes) {
    if(nodes.empty())
      return nullptr;
    else if(nodes.size() == 1)
      return nodes.front();
    PRT_ASSERT((nodes.size() % 2) == 0);
    NodeList parents;
    for(auto idx = 0; idx < nodes.size(); idx += 2)
      parents.push_back(Node::Concat(nodes[idx], nodes[idx + 1]));
    return ComputeRoot(parents);
  }

  Tree::Tree(const NodeList& leaves):
    root_(ComputeRoot(leaves)),
    leaves_(leaves) {
  }

  Tree::Tree(const std::vector<uint256>& leaves):
    Tree() {
    std::for_each(std::begin(leaves), std::end(leaves), [this](const uint256& hash) {
      leaves_.push_back(Node::New(hash));
    });
    PRT_ASSERT(leaves_.size() == leaves.size());
    if((leaves_.size() % 2) != 0)
      leaves_.push_back(Node::Copy(leaves_.back()));
    root_ = ComputeRoot(leaves_);
  }
}