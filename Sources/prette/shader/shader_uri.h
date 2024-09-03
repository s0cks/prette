#ifndef PRT_SHADER_URI_H
#define PRT_SHADER_URI_H

#include <set>
#include <string>
#include <optional>
#include "prette/uri.h"

namespace prt::shader {
  const std::set<std::string>& GetValidUriSchemes();
  bool IsValidShaderUri(const uri::Uri& uri);
  bool IsValidShaderJsonUri(const uri::Uri& uri);
  void NormalizeShaderUriPath(uri::Uri& uri);

  std::optional<std::string> GetShaderFileExtension(const uri::Uri& uri);
}

#endif //PRT_SHADER_URI_H