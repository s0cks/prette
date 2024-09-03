#include "prette/shader/shader_flags.h"

namespace prt::shader {
DEFINE_string(shader_dirs, kDefaultShaderDirs, "");

  std::string GetShaderResourcesDirectory() {
    if(!FLAGS_shader_dirs.empty())
      return FLAGS_shader_dirs;
    return FLAGS_resources;
  }
}