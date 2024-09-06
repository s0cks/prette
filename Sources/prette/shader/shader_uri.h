#ifndef PRT_SHADER_URI_H
#define PRT_SHADER_URI_H

#include <set>
#include <string>
#include <optional>
#include "prette/uri.h"

namespace prt::shader {
  auto GetValidUriSchemes() -> const std::set<std::string>&;
  auto IsValidShaderUri(const uri::Uri& uri) -> bool;
  auto IsValidShaderJsonUri(const uri::Uri& uri) -> bool;
  auto GetShaderFileExtension(const uri::Uri& uri) -> std::optional<std::string>;
  void NormalizeShaderUriPath(uri::Uri& uri);
}

#endif //PRT_SHADER_URI_H