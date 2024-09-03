#ifndef PRT_ATOMIC_LINKED_LIST_H
#define PRT_ATOMIC_LINKED_LIST_H

#include <atomic>
#include "prette/common.h"

namespace prt {
  template<typename T>
  class AtomicLinkedList {
    DEFINE_NON_COPYABLE_TYPE(AtomicLinkedList);
  public:
    struct Node {
      Node* next;
      T value;

      Node(T v):
        next(nullptr),
        value(v) {
      }
    };
  private:
    std::atomic<Node*> head_;
  public:
    AtomicLinkedList():
      head_(nullptr) {
    }
    ~AtomicLinkedList() = default;

    void Push(T* value) {
      const auto node = new Node(value);
      auto curHead = head_.load(std::memory_order_relaxed);
      do {
        node->next = curHead;
      } while(!head_.compare_exchange_weak(curHead, node, std::memory_order_release, std::memory_order_relaxed));
    }

    T Pop() {
      auto curHead = head_.load(std::memory_order_relaxed);
      while(curHead != nullptr) {
        if(head_.compare_exchange_weak(curHead, curHead->next, std::memory_order_release, std::memory_order_acquire))
          break;
      }
      const auto value = curHead->value;
      delete curHead;
      return value; 
    }

    T GetFirst() {
      return head_.load(std::memory_order_relaxed)->value;
    }

    bool IsEmpty() const {
      return head_.load(std::memory_order_relaxed) == nullptr;
    }
  };
}

#endif //PRT_ATOMIC_LINKED_LIST_H