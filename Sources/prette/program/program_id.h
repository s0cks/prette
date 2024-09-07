#ifndef PRT_PROGRAM_ID_H
#define PRT_PROGRAM_ID_H

#include "prette/gfx.h"

namespace prt {
  namespace program {
    class ProgramId {
      using RawType = GLuint;
      static constexpr const RawType kInvalid = 0;
      DEFINE_DEFAULT_COPYABLE_TYPE(ProgramId);
    private:
      RawType value_;
    public:
      constexpr ProgramId(const RawType value = kInvalid):
        value_(value) {
      }
      ~ProgramId() = default;

      constexpr auto value() const -> RawType {
        return value_;
      }

      constexpr operator RawType() const {
        return value();
      }

      constexpr operator bool() const {
        return value() >= 0;
      }

      constexpr auto operator==(const ProgramId& rhs) -> bool {
        return value() == rhs.value();
      }

      constexpr auto operator!=(const ProgramId& rhs) -> bool {
        return value() != rhs.value();
      }

      constexpr auto operator<(const ProgramId& rhs) -> bool {
        return value() < rhs.value();
      }

      constexpr auto operator>(const ProgramId& rhs) -> bool {
        return value() > rhs.value();
      }

      friend auto operator<<(std::ostream& stream, const ProgramId& rhs) -> std::ostream& {
        return stream << rhs.value();
      }
    };

    static constexpr const ProgramId kInvalidProgramId = 0;
  }
  using program::ProgramId;
  using program::kInvalidProgramId;
}

#endif //PRT_PROGRAM_ID_H