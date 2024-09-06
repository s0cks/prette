#include "prette/shader/shader_vertex.h"

#include "prette/shader/shader_json.h"
#include "prette/shader/shader_compiler.h"
#include "prette/shader/shader_unit_builder.h"

namespace prt::shader {
  const ExtensionSet VertexShader::kValidExtensions = {
    "json",
    "glsl",
    "vert",
  };

  auto VertexShader::New(ShaderUnit* unit) -> VertexShader* {
    PRT_ASSERT(unit);
    const auto& meta = unit->GetMeta();
    const auto id = ShaderCompiler::Compile(unit);
    return VertexShader::New(meta, id);
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
    if(!EndsWith(path, ".vert"))
      path = fmt::format("{0:s}.vert", path);
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


  auto VertexShader::FromSource(const uri::Uri& uri) -> VertexShader* {
    if(uri.HasScheme("shader")) {
      return FromSource(NormalizeSourceUri(uri));
    } else if(!uri.HasScheme("file")) {
      LOG(ERROR) << "invalid FragmentShader uri: " << uri;
      return nullptr;
    }
    PRT_ASSERT(uri.HasScheme("file"));
    PRT_ASSERT(uri.HasExtension(".vert"));
    const auto name = GetShaderNameFromSourceUri(uri);
    ShaderUnitBuilder builder(kVertexShader);
    builder.SetName(name);
    builder.Append(uri);
    return New(builder.Build());
  }

  auto VertexShader::FromJson(const uri::Uri& uri) -> VertexShader* {
    if(uri.HasScheme("shader")) {
      const auto shaders_dir = fmt::format("{0:s}/shaders", FLAGS_resources);
      
      auto path = uri.path;
      if(!StartsWith(path, shaders_dir))
        path = fmt::format("{0:s}/{1:s}", shaders_dir, path[0] == '/' ? path.substr(1) : path);
      if(!EndsWith(path, ".vert"))
        path = fmt::format("{0:s}.vert", path); // TODO: dont hardcode
      return FromJson(uri::Uri(fmt::format("file://{0:s}", path)));
    } else if(!uri.HasScheme("file")) {
      DLOG(ERROR) << "invalid VertexShader uri: " << uri;
      return nullptr;
    }
    DLOG(INFO) << "loading VertexShader from json file: " << uri;

    ShaderReaderHandler handler(kVertexShader);
    const auto result = json::ParseJson(uri, handler);
    if(!result) {
      LOG(ERROR) << "failed to parse VertexShader json: " << result;
      return nullptr;
    }
    ShaderUnitBuilder builder(kVertexShader);
    builder.SetMeta(handler.GetMeta());
    for(const auto& source : handler.GetSources())
      builder.Append(uri::Uri(source));
    const auto unit = builder.Build();
    return New(unit);
  }

  auto VertexShader::New(const uri::Uri& uri) -> VertexShader* {
    if(uri.HasExtension("json")) {
      return FromJson(uri);
    } else if(uri.HasExtension("vert")) {
      return FromSource(uri);
    }
    //TODO: check for json file
    //TODO: resolve to .vert file
    NOT_IMPLEMENTED(FATAL); //TODO: implement
    return nullptr;
  }

  auto VertexShader::ToString() const -> std::string {
    std::stringstream ss;
    ss << "VertexShader(";
    ss << "id=" << GetId();
    ss << ")";
    return ss.str();
  }
}