#ifndef PRT_COLOR_H
#define PRT_COLOR_H

#include <ostream>
#include "prette/glm.h"

namespace prt {
  namespace rgb {
    using Color = glm::u8vec4;

    static inline constexpr auto
    GetAlphaComponent(const uint32_t& rhs) -> uint8_t {
      return (rhs >> 24) & 0xFF;
    }

    static inline constexpr uint8_t
    GetRedComponent(const uint32_t& rhs) {
      return (rhs >> 16) & 0xFF;
    }

    static inline constexpr uint8_t
    GetGreenComponent(const uint32_t& rhs) {
      return (rhs >> 8) & 0xFF;
    }

    static inline constexpr uint8_t
    GetBlueComponent(const uint32_t& rhs) {
      return rhs & 0xFF;
    }

    static inline constexpr auto
    Hex(const uint32_t value) -> Color {
      return {
        GetRedComponent(value),
        GetGreenComponent(value),
        GetBlueComponent(value),
        0xFF
      };
    }

    enum ColorComponent : int8_t {
      kRedComponent = 0,
      kGreenComponent = 1,
      kBlueComponent = 2,
      kAlphaComponent = 3,
    };

    static inline auto
    operator<<(std::ostream& stream, const ColorComponent& rhs) -> std::ostream& {
      switch(rhs) {
        case kRedComponent: return stream << "Red (" << static_cast<int>(rhs) << ")";
        case kGreenComponent: return stream << "Green (" << static_cast<int>(rhs) << ")";
        case kBlueComponent: return stream << "Blue (" << static_cast<int>(rhs) << ")";
        case kAlphaComponent: return stream << "Alpha (" << static_cast<int>(rhs) << ")";
      }
    }

    static constexpr const Color kBlack     = Hex(0x000000);
    static constexpr const Color kWhite     = Hex(0xFFFFFF);
    static constexpr const Color kRed       = Hex(0xFF0000);
    static constexpr const Color kGreen     = Hex(0x00FF00);
    static constexpr const Color kBlue      = Hex(0x0000FF);

    static inline constexpr auto
    operator&(const Color& lhs, const Color& rhs) -> Color {
      return Color {
        lhs[kRedComponent] & rhs[kRedComponent],
        lhs[kGreenComponent] & rhs[kGreenComponent],
        lhs[kBlueComponent] & rhs[kBlueComponent],
        lhs[kAlphaComponent] & rhs[kAlphaComponent],
      };
    }

    static inline constexpr auto
    operator&(const Color& lhs, const uint32_t& rhs) -> Color {
      return Color {
        lhs[kRedComponent] & GetRedComponent(rhs),
        lhs[kGreenComponent] & GetGreenComponent(rhs),
        lhs[kBlueComponent] & GetBlueComponent(rhs),
        lhs[kAlphaComponent],
      };
    }

    static inline auto
    operator<<(std::ostream& stream, const Color& rhs) -> std::ostream& {
      stream << "rgb::Color(";
      stream << "r=" << rhs.r << ", ";
      stream << "g=" << rhs.g << ", ";
      stream << "b=" << rhs.b << ", ";
      stream << "a=" << rhs.a;
      stream << ")";
      return stream;
    }
  }
  using namespace rgb;
}

#endif //PRT_COLOR_H