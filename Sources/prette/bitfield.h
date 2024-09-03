#ifndef PRT_BITFIELD_H
#define PRT_BITFIELD_H

#include "prette/platform.h"

namespace prt {
  template<typename S, typename T, int Position, int Size=(sizeof(S)*kBitsPerByte) - Position>
  class BitField {
   public:
    static constexpr S Mask(){
      return (kUWordOne << Size) - 1;
    }

    static constexpr S MaskInPlace(){
      return Mask() << Position;
    }

    static constexpr T Decode(S val){
      const auto u = static_cast<typename std::make_unsigned<S>::type>(val);
      return static_cast<T>((u >> Position) & Mask());
    }

    static constexpr S Encode(T val){
      const auto u = static_cast<typename std::make_unsigned<S>::type>(val);
      return (u & Mask()) << Position;
    }

    static constexpr S Update(T val, S original){
      return Encode(val) | (~MaskInPlace() & original);
    }
  };

}

#endif //PRT_BITFIELD_H