#ifndef PRT_SHADER_ID_H
#define PRT_SHADER_ID_H

#include <vector>
#include "prette/gfx.h"

namespace prt {
  namespace shader {
    using ShaderId = GLuint;
    static constexpr const ShaderId kInvalidShaderId = 0;

    static inline auto
    IsInvalidShaderId(const ShaderId id) -> bool {
      return id == kInvalidShaderId;
    }

    static inline auto
    IsValidShaderId(const ShaderId id) -> bool {
      return id != kInvalidShaderId;
    }

    using ShaderIdList = std::vector<ShaderId>;
  }
  using shader::ShaderId;
  using shader::kInvalidShaderId;
  using shader::IsValidShaderId;
  using shader::IsInvalidShaderId;
  using shader::ShaderIdList;
}

#endif //PRT_SHADER_ID_H