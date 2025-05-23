#ifndef PRT_LRU_CACHE_H
#define PRT_LRU_CACHE_H

#include <concepts>
#include <optional>

#include "prette/platform.h"

#if !defined(PRT_LRUCACHE_USE_ABSL) && !defined(PRT_LRUCACHE_USE_STL)
#define PRT_LRUCACHE_USE_ABSL 1
#endif

#if defined(PRT_LRUCACHE_USE_ABSL)
#include <absl/container/flat_hash_map.h>
#elif defined(PRT_LRUCACHE_USE_STL)
#include <unordered_map>
#endif

namespace prt::lru {
template <typename K, typename V, const uint64_t Capacity, typename KeyHash, typename KeyEq>
class Cache {
 public:
  struct Node {
    Node* next = nullptr;
    Node* previous = nullptr;
    K key;
    V value;

    Node(K k, V v) :
      key(k),
      value(v) {}
  };

#if defined(PRT_LRUCACHE_USE_ABSL)
  using CacheMap = absl::flat_hash_map<K, Node*, KeyHash, KeyEq>;
#elif defined(PRT_LRUCACHE_USE_STL)
  using CacheMap = std::unordered_map<K, Node*, KeyHash, KeyEq>;
#endif

 private:
  CacheMap data_;
  Node* head_ = nullptr;
  Node* tail = nullptr;

 protected:
  inline auto GetHead() const -> Node* {
    return head_;
  }

  void Insert(Node* rhs) {
    ASSERT(rhs);
    const auto next = head_->next;
    head_->next = rhs;
    rhs->previous = head_;
    rhs->next = next;
    next->previous = rhs;
  }

  void Remove(Node* rhs) {
    auto previous = rhs->previous;
    auto next = rhs->next;
    previous->next = next;
    next->previous = previous;
  }

 public:
  Cache() = default;
  ~Cache() = default;

  auto GetCapacity() const -> uint64_t {
    return Capacity;
  }

  auto Get(const K key) const -> std::optional<V> {
    if (data_.find(key) == std::end(key))
      return std::nullopt;
    const auto node = data_[key];
    Remove(node);
    Insert(node);
    return {node->value};
  }

  void Put(const K key, const V value) {
    if (data_.find(key) != std::end(data_)) {
      const auto node = data_[key];
      Remove(node);
      delete node;
    }

    const auto node = new Node(key, value);
    data_[key] = node;
    Insert(node);

    if (data_.size() > Capacity) {
      const auto to_delete = tail->previous;
      Remove(to_delete);
      data_.erase(to_delete);
      delete to_delete;
    }
  }
};

template <typename T, typename V>
concept CacheLoaderType = requires(T t) {
  { t() } -> std::convertible_to<V>;
};

template <typename T, typename V>
concept Hasher = requires(T t, V value) {
  { t(value) } -> std::convertible_to<std::size_t>;
};

template <typename K, typename V, CacheLoaderType<V> Loader, const uint64_t Capacity, typename KeyHash, typename KeyEq>
class LoadingCache : public Cache<K, V, Capacity, KeyHash, KeyEq> {
  using CacheType = Cache<K, V, Capacity, KeyHash, KeyEq>;

 private:
  Loader* loader_;

 public:
  explicit LoadingCache(Loader* loader) :
    CacheType(),
    loader_(loader) {
    ASSERT(loader_);
  }
  ~LoadingCache() override = default;

  auto GetLoader() const -> Loader* {
    ASSERT(loader_);
    return loader_;
  }
};
}  // namespace prt::lru

#endif  // PRT_LRU_CACHE_H
