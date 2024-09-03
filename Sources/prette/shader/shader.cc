#include <sstream>
#include <unordered_map>
#include "prette/shader/shader.h"
#include "prette/shader/shader_printer.h"

namespace prt::shader {
  const std::set<std::string>& GetValidFileExtensions() {
    static std::set<std::string> kValidExtensions;
    if(kValidExtensions.empty()) {
      kValidExtensions.insert(std::begin(VertexShader::kValidExtensions), std::end(VertexShader::kValidExtensions));
      kValidExtensions.insert(std::begin(FragmentShader::kValidExtensions), std::end(FragmentShader::kValidExtensions));
      kValidExtensions.insert(std::begin(GeometryShader::kValidExtensions), std::end(GeometryShader::kValidExtensions));
      kValidExtensions.insert(std::begin(TessEvalShader::kValidExtensions), std::end(TessEvalShader::kValidExtensions));
      kValidExtensions.insert(std::begin(TessControlShader::kValidExtensions), std::end(TessControlShader::kValidExtensions));
    }
    return kValidExtensions;
  }

  static ShaderEventSubject events_;
  static ShaderSet all_;

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

  ShaderEventObservable OnEvent() {
    return events_.get_observable();
  }

  void Shader::Publish(ShaderEvent* event) {
    events_.get_subscriber().on_next(event);
  }

  const ShaderSet& GetAllShaders() {
    return all_;
  }

  uword GetTotalNumberOfShaders() {
    return all_.size();
  }

  bool VisitAllShaders(ShaderVisitor* vis) {
    PRT_ASSERT(vis);
    for(const auto& shader : all_) {
      if(!shader->Accept(vis))
        return false;
    }
    return true;
  }

#define DEFINE_VISIT_SHADERS(Name, Ext, GlValue)                        \
  bool VisitAll##Name##Shaders(ShaderVisitor* vis) {                    \
    PRT_ASSERT(vis);                                                    \
    for(const auto& shader : all_) {                                    \
      if(shader->Is##Name##Shader() && !shader->Accept(vis))            \
        return false;                                                   \
    }                                                                   \
    return true;                                                        \
  }
  FOR_EACH_SHADER_TYPE(DEFINE_VISIT_SHADERS);
#undef DEFINE_VISIT_SHADERS

  bool PrintAllShaders(const google::LogSeverity severity) {
    DLOG(INFO) << GetTotalNumberOfShaders() << " Shaders:";
    ShaderPrinter printer(severity, __FILE__, __LINE__, 0);
    return VisitAllShaders(&printer);
  }

#define DEFINE_PRINT_ALL_SHADERS(Name, Ext, GlValue)                   \
  bool PrintAll##Name##Shaders(const google::LogSeverity severity) {   \
    ShaderPrinter printer(severity, __FILE__, __LINE__, 0);            \
    return VisitAll##Name##Shaders(&printer);                          \
  }
  FOR_EACH_SHADER_TYPE(DEFINE_PRINT_ALL_SHADERS)
#undef DEFINE_PRINT_ALL_SHADERS
}