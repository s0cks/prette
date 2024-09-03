#ifndef PRT_SHADER_ID_H
#define PRT_SHADER_ID_H

#include <vector>
#include "prette/gfx.h"

namespace prt {
  namespace shader {
    typedef GLuint ShaderId;
    static constexpr const ShaderId kInvalidShaderId = 0;

    static inline bool
    IsInvalidShaderId(const ShaderId id) {
      return id == kInvalidShaderId;
    }

    static inline bool
    IsValidShaderId(const ShaderId id) {
      return id != kInvalidShaderId;
    }

    typedef std::vector<ShaderId> ShaderIdList;
  }
  using shader::ShaderId;
  using shader::kInvalidShaderId;
  using shader::IsValidShaderId;
  using shader::IsInvalidShaderId;
  using shader::ShaderIdList;
}

#endif //PRT_SHADER_ID_H