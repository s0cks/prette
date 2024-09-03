#ifndef PRT_CACHE_H
#define PRT_CACHE_H

#include <optional>
#include <functional>

#include "prette/common.h"
#include "prette/platform.h"

namespace prt {
  struct DefaultHasher {
    uint32_t operator()(const std::string& key) const {
      NOT_IMPLEMENTED(FATAL); //TODO: implement
      return 0; //TODO: Murmur(key.data(), key.size());
    }
  };

  static constexpr const uint64_t kDefaultNumberOfLruCacheBuckets = 10;
  template<typename K, typename V, typename Hasher>
  class Cache {
  protected:
    Cache() = default;
  public:
    Cache(const Cache& rhs) = delete;
    virtual ~Cache() = default;
    virtual V Get(const K& key, const std::function<V(const K&)>& load) = 0;
    virtual uint64_t GetSize() const = 0;
    virtual uint64_t GetCapacity() const = 0;

    void operator=(const Cache& rhs) = delete;
  public:
    template<const uint64_t NumberOfBuckets = kDefaultNumberOfLruCacheBuckets>
    static Cache<K, V, Hasher>* NewLru(const uint64_t capacity);
  };

  template<typename K, typename V, typename Hasher = DefaultHasher, const uint64_t NumberOfBuckets = kDefaultNumberOfLruCacheBuckets>
  class LruCache : public Cache<K, V, Hasher> {
    friend class Cache<K, V, Hasher>;
  private:
    struct Node {
      Node* next;
      Node* previous;
      uint32_t key;
      V value;
    };
  protected:
    Node* nodes_[NumberOfBuckets];
    Node* head_;
    Node* tail_;
    uint64_t size_;
    uint64_t capacity_;
    Hasher hasher_;

    static inline uint32_t
    GetIndex(const uint32_t hash) {
      return hash % NumberOfBuckets;
    }

    inline void
    RemoveNode(Node* node) {
      if(node == head_) {
        head_ = node->next;
      } else if(node == tail_) {
        tail_ = node->previous;
      } else {
        const auto previous = node->previous;
        const auto next = node->next;
        if(previous)
          previous->next = next;
        if(next)
          next->previous = previous;
      }
      size_ -= 1;
    }

    inline void
    PushFront(Node* node) {
      if(head_ == nullptr) {
        head_ = tail_ = node;
      } else {
        node->next = head_;
        if(head_)
          head_->previous = node;
        head_ = node;
      }
      size_ += 1;
    }
  public:
    LruCache():
      nodes_(),
      head_(nullptr),
      tail_(nullptr),
      size_(0),
      capacity_(0) {
      for(auto i = 0; i < NumberOfBuckets; i++)
        nodes_[i] = nullptr;
    }
    explicit LruCache(const uint64_t capacity):
      nodes_(),
      head_(nullptr),
      tail_(nullptr),
      size_(0),
      capacity_(capacity) {
      for(auto i = 0; i < NumberOfBuckets; i++)
        nodes_[i] = nullptr;
    }
    LruCache(const LruCache& rhs) = delete;
    ~LruCache() override = default;

    uint64_t GetSize() const override {
      return size_;
    }

    uint64_t GetCapacity() const override {
      return capacity_;
    }

    V Get(const K& key, const std::function<V(const K&)>& load) override {
      const auto h = hasher_(key);
      const auto index = GetIndex(h);
      auto node = nodes_[index];
      while(node != nullptr) {
        if(node->key == h) {
          RemoveNode(node);
          PushFront(node);
          return node->value;
        }
        node = node->next;
      }

      if((size_ + 1) >= capacity_) {
        const auto tail = tail_;
        RemoveNode(tail_);
        delete tail;
      }

      const auto value = load(key);
      node = new Node();
      node->key = h;
      node->value = value;
      PushFront(node);
      return value;
    }

    void operator=(const LruCache& rhs) = delete;
  };

  template<typename K, typename V, typename Hasher>
  template<const uint64_t NumberOfBuckets>
  Cache<K, V, Hasher>* Cache<K, V, Hasher>::NewLru(const uint64_t capacity) {
    return new LruCache<K, V, Hasher, NumberOfBuckets>(capacity);
  }
}

#endif //PRT_CACHE_H