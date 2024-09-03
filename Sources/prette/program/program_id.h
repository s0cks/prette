#ifndef PRT_PROGRAM_ID_H
#define PRT_PROGRAM_ID_H

#include "prette/gfx.h"

namespace prt {
  namespace program {
    typedef GLuint ProgramId;
    static constexpr const ProgramId kInvalidProgramId = 0;

    static inline bool
    IsValidProgramId(const ProgramId id) {
      return id != kInvalidProgramId;
    }
  }
  using program::ProgramId;
  using program::kInvalidProgramId;
}

#endif //PRT_PROGRAM_ID_H