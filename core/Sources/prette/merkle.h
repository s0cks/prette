#ifndef PRT_MERKLE_H
#define PRT_MERKLE_H

#include <algorithm>
#include <concepts>
#include <string>
#include <type_traits>
#include <vector>

#include "prette/common.h"
#include "prette/hash/murmur3.h"
#include "prette/sha256.h"
#include "prette/uint256.h"

namespace prt {
namespace merkle {
template <typename T>
struct is_hash_t : std::false_type {};

template <typename T>
concept HashType = is_hash_t<T>::value;

template <>
struct is_hash_t<uint32_t> : std::true_type {};

template <>
struct is_hash_t<uint256> : std::true_type {};

template <typename T, typename H>
concept HashGenFunc = requires(T value, const uint8_t* bytes, const uint64_t num_bytes) {
  { value(bytes, num_bytes) } -> std::convertible_to<H>;
};

template <typename T, typename H>
concept HashConcatFunc = requires(T value, const H& lhs, const H& rhs) {
  { value(lhs, rhs) } -> std::convertible_to<H>;
};

template <typename T, HashGenFunc<T> HashGen, HashConcatFunc<T> HashConcat>
struct HashClassTemplate {
  using HashType = T;
  using HashGenFunc = HashGen;
  using HashConcatFunc = HashConcat;
};

template <typename T, typename Hash>
concept HashClass = requires(T value) {
  HashType<Hash>;
  typename T::HashType;
  typename T::HashGenFunc;
  typename T::HashConcatFunc;
};

template <HashType H, HashClass<H> C>
static inline auto ComputeMerkleRoot(const std::vector<H> leaves) -> H {
  if (leaves.empty())
    return H{};
  std::vector<H> previous = leaves;
  std::vector<H> current{};
  while (previous.size() > 1) {
    current.clear();
    for (auto idx = 0; idx < previous.size(); idx += 2) {
      typename C::HashConcatFunc concat{};
      if ((idx + 1) < previous.size()) {
        current.push_back(concat(previous[idx], previous[idx + 1]));
      } else {
        current.push_back(concat(previous[idx], previous[idx]));
      }
    }
    previous = current;
  }
  return previous[0];
}

template <HashType H, HashClass<H> C>
static inline auto ComputeMerkleRoot(const std::vector<std::string> leaves) -> H {
  if (leaves.empty())
    return H{};
  std::vector<H> data{};
  data.resize(leaves.size());
  typename C::HashGenFunc gen{};
  for (auto idx = 0; idx < leaves.size(); idx++)
    data[idx] =
        gen((const uint8_t*)leaves[idx].c_str(), leaves[idx].size());  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
  return ComputeMerkleRoot<H, C>(std::move(data));
}
}  // namespace merkle

namespace murmur3_32 {

struct ConcatHash {
  auto operator()(const Digest& lhs, const Digest& rhs) const -> Digest {
    uint64_t combined = (static_cast<uint64_t>(lhs) << 32) | rhs;
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
    return of((const uint8_t*)&combined, sizeof(uint64_t));
  }
};

struct GenHash {
  auto operator()(const uint8_t* bytes, const uint64_t num_bytes) -> Digest {
    return of(bytes, num_bytes);
  }
};

struct HashClass : public prt::merkle::HashClassTemplate<Digest, GenHash, ConcatHash> {
  static constexpr const auto kClassName = "murmur32";
};

static inline auto ComputeMerkleRoot(const std::vector<Digest> leaves) -> Digest {
  return prt::merkle::ComputeMerkleRoot<Digest, HashClass>(leaves);
}

static inline auto ComputeMerkleRoot(const std::vector<std::string>& leaves) -> Digest {
  return prt::merkle::ComputeMerkleRoot<Digest, HashClass>(leaves);
}
}  // namespace murmur3_32

namespace sha256 {
struct ConcatHash {
  auto operator()(const Digest& lhs, const Digest& rhs) const -> Digest {
    return sha256::Concat(lhs, rhs);
  }
};

struct GenHash {
  auto operator()(const uint8_t* bytes, const uint64_t num_bytes) -> Digest {
    return Of(bytes, num_bytes);
  }
};

struct HashClass : public prt::merkle::HashClassTemplate<Digest, GenHash, ConcatHash> {
  static constexpr const auto kClassName = "sha256";
};

static inline auto ComputeMerkleRoot(const std::vector<Digest> leaves) -> Digest {
  return prt::merkle::ComputeMerkleRoot<Digest, HashClass>(leaves);
}

static inline auto ComputeMerkleRoot(const std::vector<std::string>& leaves) -> Digest {
  return prt::merkle::ComputeMerkleRoot<Digest, HashClass>(leaves);
}
}  // namespace sha256
}  // namespace prt

#endif  // PRT_MERKLE_H
