#ifndef PRT_REGISTRY_H
#define PRT_REGISTRY_H

// clang-format off
#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/hashcode.h"
// clang-format on

#include <concepts>
#include <functional>
#include <string>
#include <uuid.h>

#if !defined(PRT_USE_ABSL) && !defined(PRT_USE_STL)
#define PRT_USE_ABSL 1
#endif

#if defined(PRT_USE_ABSL)

#include <absl/container/flat_hash_map.h>

#elif defined(PRT_USE_STL)

#include <unordered_map>

#endif

namespace prt::registry {
using Key = uuids::uuid;

template <class T>
concept HasId = requires(T lhs, T rhs) {
  { lhs.GetId() } -> std::convertible_to<Key>;
};

template <class T>
concept HasHashCode = requires(T lhs, T rhs) {
  { lhs.GetHashCode() } -> std::convertible_to<HashCode>;
};

template <class T>
concept HasEquals = requires(T lhs, T rhs) {
  { lhs.Equals(rhs) } -> std::convertible_to<bool>;
};

template <class T>
concept HasToString = requires(T value) {
  { value.ToString() } -> std::convertible_to<std::string>;
};

template <class T>
concept Registerable = HasId<T> && HasHashCode<T> && HasEquals<T> && HasToString<T>;

template <Registerable R>
class Registry {
  DEFINE_NON_COPYABLE_TYPE(Registry);
#if defined(PRT_USE_ABSL)
  using RegisterableMap = absl::flat_hash_map<Key, R*, std::hash<Key>, std::equal_to<Key>>;
#elif defined(PRT_USE_STL)
  using RegisterableMap = std::unordered_map<Key, R*, std::hash<Key>, Eq>;
#else
#error ""
#endif

  using RegisterableProvider = std::function<R*(const Key&)>;

 private:
  RegisterableMap data_{};

 public:
  Registry() = default;
  ~Registry() = default;

  auto data() const -> const RegisterableMap& {
    return data_;
  }

  auto Get(const Key& k) const -> R* {
    const auto pos = data().find(k);
    return pos != std::end(data()) ? pos->second : (R*)nullptr;
  }

  inline auto Contains(const Key& k) const -> bool {
    return Get(k) != nullptr;
  }

  auto Put(const R& v) -> bool {
    const auto [_, success] = data_.insert({v.GetId(), new R(v)});
    return success;
  }

  auto GetOrDefault(const Key& k, const RegisterableProvider& default_provider) -> R* {
    const auto pos = data_.find(k);
    if (pos != std::end(data_))
      return (*pos);
    const auto new_value = default_provider(k);
    ASSERT(new_value);
    const auto [new_pos, success] = data_.insert({k, new_value});
    LOG_IF(FATAL, !success) << "failed to insert default value " << new_value->ToString() << " w/ key " << k;
    return (*new_pos);
  }
};
}  // namespace prt::registry

#endif  // PRT_REGISTRY_H
