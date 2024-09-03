#ifndef PRT_SHADER_FLAGS_H
#define PRT_SHADER_FLAGS_H

#include <set>
#include "prette/flags.h"

namespace prt::shader {
static constexpr const auto kDefaultShaderDirs = "";
DECLARE_string(shader_dirs);

  std::string GetShaderResourcesDirectory();
}

#endif //PRT_SHADER_FLAGS_H