#include "prette/merkle.h"

#include <algorithm>
#include <vector>

#include "prette/common.h"
#include "prette/uint256.h"

namespace prt::merkle {
auto Tree::ComputeRoot(const NodeList& nodes) -> Node* {
  if (nodes.empty())
    return nullptr;
  else if (nodes.size() == 1)
    return nodes.front();
  ASSERT((nodes.size() % 2) == 0);
  NodeList parents;
  for (auto idx = 0; idx < nodes.size(); idx += 2)
    parents.push_back(Node::Concat(nodes[idx], nodes[idx + 1]));
  return ComputeRoot(parents);
}

Tree::Tree(const NodeList& leaves) :
  root_(ComputeRoot(leaves)),
  leaves_(leaves) {}

Tree::Tree(const std::vector<uint256>& leaves) :
  Tree() {
  std::ranges::for_each(leaves, [this](const uint256& hash) {
    leaves_.push_back(Node::New(hash));
  });
  ASSERT(leaves_.size() == leaves.size());
  if ((leaves_.size() % 2) != 0)
    leaves_.push_back(Node::Copy(leaves_.back()));
  root_ = ComputeRoot(leaves_);
}
}  // namespace prt::merkle