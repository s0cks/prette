#include "prette/shader/shader_fragment.h"
#include "prette/shader/shader_unit.h"
#include "prette/shader/shader_json.h"
#include "prette/shader/shader_compiler.h"
#include "prette/shader/shader_unit_builder.h"

namespace prt::shader {
  const ExtensionSet FragmentShader::kValidExtensions = {
    "json",
    "glsl",
    "frag",
  };

  auto FragmentShader::ToString() const -> std::string {
    std::stringstream ss;
    ss << "FragmentShader(";
    ss << "id=" << GetId() << ", ";
    ss << "meta=" << GetMeta();
    ss << ")";
    return ss.str();
  }

  auto FragmentShader::New(ShaderUnit* unit) -> FragmentShader* {
    PRT_ASSERT(unit);
    const auto& meta = unit->GetMeta();
    const auto id = ShaderCompiler::Compile(unit);
    return New(meta, id);
  }

  auto FragmentShader::New(const uri::Uri& uri) -> FragmentShader* {
    if(uri.HasExtension("json")) {
      return FromJson(uri);
    } else if(uri.HasExtension("frag")) {
      return FromSource(uri);
    }

    //TODO: check for .json
    //TODO: check for .frag
    NOT_IMPLEMENTED(FATAL); //TODO: implement
    return nullptr;
  }

  static inline auto
  NormalizeSourceUri(const uri::Uri& uri) -> uri::Uri {
    const auto shaders_dir = fmt::format("{0:s}/shaders", FLAGS_resources);
    uri::Uri normalized(uri);
    if(!normalized.HasScheme("file"))
      normalized.scheme = "file";
    auto& path = normalized.path;
    if(!StartsWith(shaders_dir, path))
      path = fmt::format("{0:s}/{1:s}", shaders_dir, path);
    if(!EndsWith(path, ".frag"))
      path = fmt::format("{0:s}.frag", path);
    return normalized;
  }

  static inline auto
  GetShaderNameFromSourceUri(const uri::Uri& uri) -> std::string {
    const auto& path = uri.path;
    auto name = path;
    const auto slashpos = name.find_last_of('/');
    if(slashpos != std::string::npos)
      name = name.substr(slashpos + 1);
    const auto dotpos = name.find_last_of('.');
    if(dotpos != std::string::npos)
      name = name.substr(0, name.length() - dotpos);
    return name;
  }

  auto FragmentShader::FromSource(const uri::Uri& uri) -> FragmentShader* {
    if(uri.HasScheme("shader")) {
      return FromSource(NormalizeSourceUri(uri));
    } else if(!uri.HasScheme("file")) {
      LOG(ERROR) << "invalid FragmentShader uri: " << uri;
      return nullptr;
    }
    PRT_ASSERT(uri.HasScheme("file"));
    PRT_ASSERT(uri.HasExtension(".frag"));
    const auto name = GetShaderNameFromSourceUri(uri);
    ShaderUnitBuilder builder(kFragmentShader);
    builder.SetName(name);
    builder.Append(uri);
    return New(builder.Build());
  }

  static inline auto
  NormalizeJsonUri(const uri::Uri& uri) -> uri::Uri {
    const auto shaders_dir = fmt::format("{0:s}/shaders", FLAGS_resources);
    uri::Uri normalized(uri);
    if(!normalized.HasScheme("file"))
      normalized.scheme = "file";
    auto& path = normalized.path;
    if(!StartsWith(shaders_dir, path))
      path = fmt::format("{0:s}/{1:s}", shaders_dir, path);
    if(!EndsWith(path, ".json"))
      path = fmt::format("{0:s}.json", path);
    return normalized;
  }

  auto FragmentShader::FromJson(const uri::Uri& uri) -> FragmentShader* {
    auto target = NormalizeJsonUri(uri);
    ShaderReaderHandler handler(kFragmentShader);
    const auto result = json::ParseJson(uri, handler);
    if(!result) {
      LOG(ERROR) << "failed to parse VertexShader json: " << result;
      return nullptr;
    }
    ShaderUnitBuilder builder(kFragmentShader);
    builder.SetMeta(handler.GetMeta());
    for(const auto& source : handler.GetSources())
      builder.Append(uri::Uri(source));
    const auto unit = builder.Build();
    return New(unit);
  }

  auto FragmentShader::FromJson(const std::string& value) -> FragmentShader* {
    NOT_IMPLEMENTED(FATAL); //TODO: implement
    return nullptr;
  }
}