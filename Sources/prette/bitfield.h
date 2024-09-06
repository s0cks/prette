#ifndef PRT_BITFIELD_H
#define PRT_BITFIELD_H

#include <type_traits>
#include "prette/platform.h"

namespace prt {
  template<typename S, typename T, int Position, int Size=(sizeof(S)*kBitsPerByte) - Position>
  class BitField {
   public:
    static constexpr auto Mask() -> S{
      return (kUWordOne << Size) - 1;
    }

    static constexpr auto MaskInPlace() -> S{
      return Mask() << Position;
    }

    static constexpr auto Decode(S val) -> T{
      const auto u = static_cast<typename std::make_unsigned<S>::type>(val);
      return static_cast<T>((u >> Position) & Mask());
    }

    static constexpr auto Encode(T val) -> S{
      const auto u = static_cast<typename std::make_unsigned<S>::type>(val);
      return (u & Mask()) << Position;
    }

    static constexpr auto Update(T val, S original) -> S{
      return Encode(val) | (~MaskInPlace() & original);
    }
  };
}

#endif //PRT_BITFIELD_H