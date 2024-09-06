#include "prette/shader/shader_flags.h"

namespace prt::shader {
// NOLINTNEXTLINE
DEFINE_string(shader_dirs, kDefaultShaderDirs, "");

  auto GetShaderResourcesDirectory() -> std::string {
    if(!FLAGS_shader_dirs.empty())
      return FLAGS_shader_dirs;
    return FLAGS_resources;
  }
}