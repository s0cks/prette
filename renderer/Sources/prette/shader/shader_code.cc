#include "prette/shader/shader_code.h"

#include <fstream>
#include <units.h>

#include "prette/common.h"

namespace prt::vk {
ShaderCode::ShaderCode(fs::path path) :
  path_(path) {
  if (!fs::exists(path)) {
    LOG(ERROR) << "failed to find ShaderCode at: " << path;
    return;
  }
  std::ifstream file(path, std::ios::ate | std::ios::binary);
  if (!file.is_open()) {
    LOG(ERROR) << "failed to open ShaderCode at: " << path;
    return;
  }
  auto filesize = static_cast<std::streamsize>(file.tellg());
  DVLOG(1) << "loading shader code from " << path << " (" << units::data::byte_t(static_cast<double>(filesize))
           << ")...";
  buffer_.resize(filesize);
  file.seekg(0);
  file.read((char*)buffer_.data(), filesize);  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
  file.close();
}
}  // namespace prt::vk