#include <sstream>
#include <unordered_map>
#include "prette/shader/shader.h"
#include "prette/shader/shader_printer.h"

#include "prette/shader/shader_vertex.h"
#include "prette/shader/shader_fragment.h"
#include "prette/shader/shader_geometry.h"
#include "prette/shader/shader_tess_eval.h"
#include "prette/shader/shader_tess_control.h"

namespace prt::shader {
  auto GetValidFileExtensions() -> const ExtensionSet& {
    static ExtensionSet kValidExtensions;
    if(kValidExtensions.empty()) {
      kValidExtensions.insert(std::begin(VertexShader::kValidExtensions), std::end(VertexShader::kValidExtensions));
      kValidExtensions.insert(std::begin(FragmentShader::kValidExtensions), std::end(FragmentShader::kValidExtensions));
      kValidExtensions.insert(std::begin(GeometryShader::kValidExtensions), std::end(GeometryShader::kValidExtensions));
      kValidExtensions.insert(std::begin(TessEvalShader::kValidExtensions), std::end(TessEvalShader::kValidExtensions));
      kValidExtensions.insert(std::begin(TessControlShader::kValidExtensions), std::end(TessControlShader::kValidExtensions));
    }
    return kValidExtensions;
  }

  // NOLINTBEGIN
  static ShaderEventSubject events_;
  static ShaderSet all_;
  // NOLINTEND

  static inline void
  Register(Shader* shader) {
    PRT_ASSERT(shader);
    const auto pos = all_.insert(shader);
    LOG_IF(ERROR, !pos.second) << "failed to register: " << shader->ToString();
  }

  static inline void
  Deregister(Shader* shader) {
    PRT_ASSERT(shader);
    const auto removed = all_.erase(shader);
    LOG_IF(ERROR, removed != 1) << "failed to deregister: " << shader->ToString();
  }

  Shader::Shader(const Metadata& meta, const ShaderId id):
    Object(meta),
    id_(id) {
    Register(this);
  }

  Shader::~Shader() {
    Deregister(this);
  }

  auto OnEvent() -> ShaderEventObservable {
    return events_.get_observable();
  }

  void Shader::Publish(ShaderEvent* event) {
    events_.get_subscriber().on_next(event);
  }

  auto GetAllShaders() -> const ShaderSet& {
    return all_;
  }

  auto GetTotalNumberOfShaders() -> uword {
    return all_.size();
  }

  auto VisitAllShaders(ShaderVisitor* vis) -> bool {
    PRT_ASSERT(vis);
    for(const auto& shader : all_) {
      if(!shader->Accept(vis))
        return false;
    }
    return true;
  }

#define DEFINE_VISIT_SHADERS(Name, Ext, GlValue)                        \
  auto VisitAll##Name##Shaders(ShaderVisitor* vis) -> bool {            \
    PRT_ASSERT(vis);                                                    \
    for(const auto& shader : all_) {                                    \
      if(shader->Is##Name##Shader() && !shader->Accept(vis))            \
        return false;                                                   \
    }                                                                   \
    return true;                                                        \
  }
  FOR_EACH_SHADER_TYPE(DEFINE_VISIT_SHADERS);
#undef DEFINE_VISIT_SHADERS

  auto PrintAllShaders(const google::LogSeverity severity) -> bool {
    DLOG(INFO) << GetTotalNumberOfShaders() << " Shaders:";
    ShaderPrinter printer(severity, __FILE__, __LINE__, 0);
    return VisitAllShaders(&printer);
  }

#define DEFINE_PRINT_ALL_SHADERS(Name, Ext, GlValue)                            \
  auto PrintAll##Name##Shaders(const google::LogSeverity severity) -> bool {    \
    ShaderPrinter printer(severity, __FILE__, __LINE__, 0);                     \
    return VisitAll##Name##Shaders(&printer);                                   \
  }
  FOR_EACH_SHADER_TYPE(DEFINE_PRINT_ALL_SHADERS)
#undef DEFINE_PRINT_ALL_SHADERS
}