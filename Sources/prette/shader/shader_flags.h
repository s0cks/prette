#ifndef PRT_SHADER_FLAGS_H
#define PRT_SHADER_FLAGS_H

#include <set>
#include "prette/flags.h"

namespace prt::shader {
static constexpr const auto kDefaultShaderDirs = "";
// NOLINTNEXTLINE
DECLARE_string(shader_dirs);

  auto GetShaderResourcesDirectory() -> std::string;
}

#endif //PRT_SHADER_FLAGS_H