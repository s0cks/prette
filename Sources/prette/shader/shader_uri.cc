#include "prette/shader/shader_uri.h"
#include "prette/shader/shader.h"
#include "prette/shader/shader_flags.h"

namespace prt::shader {
  auto
  GetValidUriSchemes() -> const std::set<std::string>& {
    static std::set<std::string> kValidSchemes;
    if(kValidSchemes.empty()) {
      kValidSchemes.insert("shader");
      kValidSchemes.insert("file");
    }
    return kValidSchemes;
  }

  auto IsValidShaderUri(const uri::Uri& uri) -> bool {
    return uri.HasScheme(GetValidUriSchemes());
  }

  auto IsValidShaderJsonUri(const uri::Uri& uri) -> bool {
    return IsValidShaderUri(uri) && uri.HasExtension("json");
  }

  void NormalizeShaderUriPath(uri::Uri& uri) {
    const auto root = GetShaderResourcesDirectory();
    auto& path = uri.path;
    if(StartsWith(path, root))
      return;
    path = fmt::format("{0:s}/{1:s}", root, path);
  }

  auto GetShaderFileExtension(const uri::Uri& uri) -> std::optional<std::string> {
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