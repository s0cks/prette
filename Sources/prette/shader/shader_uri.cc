#include "prette/shader/shader_uri.h"
#include "prette/shader/shader.h"
#include "prette/shader/shader_flags.h"

namespace prt::shader {
  const std::set<std::string>&
  GetValidUriSchemes() {
    static std::set<std::string> kValidSchemes;
    if(kValidSchemes.empty()) {
      kValidSchemes.insert("shader");
      kValidSchemes.insert("file");
    }
    return kValidSchemes;
  }

  bool IsValidShaderUri(const uri::Uri& uri) {
    return uri.HasScheme(GetValidUriSchemes());
  }

  bool IsValidShaderJsonUri(const uri::Uri& uri) {
    return IsValidShaderUri(uri) && uri.HasExtension("json");
  }

  void NormalizeShaderUriPath(uri::Uri& uri) {
    const auto root = GetShaderResourcesDirectory();
    auto& path = uri.path;
    if(StartsWith(path, root))
      return;
    path = fmt::format("{0:s}/{1:s}", root, path);
  }

  std::optional<std::string> GetShaderFileExtension(const uri::Uri& uri) {
    PRT_ASSERT(uri.HasScheme("file"));
    const auto extension = uri.GetExtension();
    if(extension.empty())
      return std::nullopt;
    const auto& extensions = shader::GetValidFileExtensions();
    const auto pos = extensions.find(extension);
    if(pos == std::end(extensions))
      return std::nullopt;
    return { extension };
  }
}