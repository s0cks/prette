#include "prette/tag.h"
#include "prette/thread_local.h"

namespace prt {
  static TagTrie tags_;

  Tag* Tag::Of(const std::string& value) {
    return tags_.Search(value);
  }

  Tag* TagTrie::Search(Node* root, const std::string& value) {
    auto node = root;
    auto iter = value.begin();
    do {
      const auto& c = (*iter++);
      if(!node->HasChildAt(c)) {
        const auto new_node = new Node();
        node->SetChildAt(c, new_node);
        node = new_node;
      }
    } while(iter != value.end());

    PRT_ASSERT(node);
    if(node->HasValue())
      return node->GetValue();
    const auto new_value = new Tag(value);
    node->SetValue(new_value);
    return new_value;
  }
}