#include "prette/json.h"

#include <fstream>
#include <string>

#include "prette/common.h"

// IWYU pragma: no_include <ios>

namespace prt::json {
auto ReadJsonFromFile(fs::path path, std::string& result) -> bool {
  if (!fs::exists(path)) {
    LOG(ERROR) << "failed to find ShaderCode at: " << path;
    return false;
  }
  std::ifstream file(path, std::ios::ate | std::ios::binary);
  if (!file.is_open()) {
    LOG(ERROR) << "failed to open ShaderCode at: " << path;
    return false;
  }
  auto filesize = static_cast<std::streamsize>(file.tellg());
  result.resize(filesize + 1);
  file.seekg(0);
  file.read((char*)result.data(), filesize);
  file.close();
  result[filesize] = '\0';
  return true;
}
}  // namespace prt::json