#include "prette/shader/shader.h"

#include "prette/shader/shader_json.h"
#include "prette/shader/shader_compiler.h"

#include "prette/shader/shader_unit_builder.h"

namespace prt::shader {
  const std::set<std::string> VertexShader::kValidExtensions = {
    "json",
    "glsl",
    "vert",
  };

  VertexShader* VertexShader::New(ShaderUnit* unit) {
    PRT_ASSERT(unit);
    const auto& meta = unit->GetMeta();
    const auto id = ShaderCompiler::Compile(unit);
    return VertexShader::New(meta, id);
  }

  static inline uri::Uri
  NormalizeSourceUri(const uri::Uri& uri) {
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

  static inline std::string
  GetShaderNameFromSourceUri(const uri::Uri& uri) {
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


  VertexShader* VertexShader::FromSource(const uri::Uri& uri) {
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

  VertexShader* VertexShader::FromJson(const uri::Uri& uri) {
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

  VertexShader* VertexShader::New(const uri::Uri& uri) {
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

  std::string VertexShader::ToString() const {
    std::stringstream ss;
    ss << "VertexShader(";
    ss << "id=" << GetId();
    ss << ")";
    return ss.str();
  }
}