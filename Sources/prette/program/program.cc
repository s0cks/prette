#include "prette/program/program.h"

#include "prette/buffer.h"
#include "prette/json_schema.h"

#include "prette/shader/shader_code.h"
#include "prette/shader/shader_compiler.h"

#include "prette/program/program_json.h"
#include "prette/program/program_builder.h"

namespace prt::program {
  static ProgramSet all_;
  static rx::subject<ProgramEvent*> events_;

  static inline void
  Register(Program* p) {
    PRT_ASSERT(p);
    const auto pos = all_.insert(p);
    LOG_IF(ERROR, !pos.second) << "failed to register: " << p->ToString();
  }

  static inline void
  Deregister(Program* p) {
    PRT_ASSERT(p);
    const auto removed = all_.erase(p);
    LOG_IF(ERROR, removed != 1) << "failed to deregister: " << p->ToString();
  }

  rx::observable<ProgramEvent*> OnProgramEvent() {
    return events_.get_observable();
  }

  void Program::Publish(ProgramEvent* event) {
    PRT_ASSERT(event);
    const auto& subscriber = events_.get_subscriber();
    subscriber.on_next(event);
  }

  void Program::UseProgram(const ProgramId id) {
    PRT_ASSERT(IsValidProgramId(id) || id == 0);
    glUseProgram(id);
    CHECK_GL;
  }

  void Program::DeleteProgram(const ProgramId id) {
    PRT_ASSERT(IsValidProgramId(id) && id != 0); //TODO: better assertion
    glDeleteProgram(id);
    CHECK_GL;
  }

  void Program::Attach(const ProgramId programId, const ShaderId shaderId) {
    glAttachShader(programId, shaderId);
    CHECK_GL;
  }

  void Program::Detach(const ProgramId programId, const ShaderId shaderId) {
    glDetachShader(programId, shaderId);
    CHECK_GL;
  }

  Program::Program(const Metadata& meta, const ProgramId id):
    gfx::Object<ProgramId>(id) {
    SetMeta(meta);
    Register(this);
  }

  Program::~Program() {
    Deregister(this);
  }

  int Program::GetProgramiv(const Property property) const {
    GLint value;
    glGetProgramiv(GetId(), property, &value);
    return static_cast<int>(value);
  }

  bool Program::VisitAllAttributes(AttributeVisitor* vis) const {
    AttributeIterator iter(this);
    while(iter.HasNext()) {
      const auto next = iter.Next();
      if(!vis->VisitAttribute(next))
        return false;
    }
    return true;
  }

  bool Program::VisitAllUniforms(UniformVisitor* vis) const {
    UniformIterator iter(this);
    while(iter.HasNext()) {
      const auto next = iter.Next();
      if(!vis->VisitUniform(next))
        return false;
    }
    return true;
  }

  rx::observable<ProgramAttribute> Program::GetActiveAttributes() const {
    return rx::observable<>::create<ProgramAttribute>([this](rx::subscriber<ProgramAttribute> s) {
      AttributeIterator iter(this);
      while(iter.HasNext()) {
        const auto next = iter.Next();
        s.on_next(next);
      }
      s.on_completed();
    });
  }

  rx::observable<ProgramUniform> Program::GetActiveUniforms() const {
    return rx::observable<>::create<ProgramUniform>([this](rx::subscriber<ProgramUniform> s) {
      UniformIterator iter(this);
      while(iter.HasNext()) {
        const auto next = iter.Next();
        s.on_next(next);
      }
      s.on_completed();
    });
  }

  std::string Program::ToString() const {
    std::stringstream ss;
    ss << "Program(";
    ss << "meta=" << GetMeta() << ", ";
    ss << "id=" << GetId();
    ss << ")";
    return ss.str();
  }

  bool Program::Accept(ProgramVisitor* vis) {
    PRT_ASSERT(vis);
    return vis->VisitProgram(this);
  }

  ProgramEventObservable Program::OnEvent() const {
    return events_.get_observable();
  }

  static inline std::string
  GetProgramNameFromDirectory(const std::string& dir) {
    const auto slashpos = dir.find_last_of('/');
    if(slashpos == std::string::npos)
      return {};
    return {};
  }

  Program* Program::FromJson(const uri::Uri& uri) {
    if(uri.HasScheme("program")) {
      const auto programs_dir = fmt::format("{0:s}/shaders", FLAGS_resources);

      auto path = uri.path;
      if(!StartsWith(path, programs_dir))
        path = fmt::format("{0:s}/{1:s}", programs_dir, path[0] == '/' ? path.substr(1) : path);
      
      if(!EndsWith(path, ".json"))
        path = fmt::format("{0:s}.json", path);
      const auto new_uri = uri::Uri(fmt::format("file://{0:s}", path));
      return FromJson(new_uri);
    } else if(!uri.HasScheme("file")) {
      LOG(ERROR) << "invalid Program uri: " << uri;
      return nullptr;
    }
    PRT_ASSERT(uri.HasScheme("file"));
    PRT_ASSERT(uri.HasExtension("json"));
    DLOG(INFO) << "getting program from: " << uri;

    ProgramReaderHandler handler;
    const auto result = json::ParseJson<ProgramReaderHandler>(uri, handler);
    if(!result) {
      LOG(ERROR) << result;
      return nullptr;
    }

    ProgramBuilder builder;
    builder.SetMeta(handler.GetMeta());

    const auto& shaders = handler.shaders();
    DLOG(INFO) << "shaders:";
    for(const auto& shader : shaders) {
      DLOG(INFO) << " - " << shader;
      builder.Attach(shader.GetShader());
    }
    return builder.Build();
  }

  const ProgramSet& GetAllPrograms() {
    return all_;
  }
  
  uword GetTotalNumberOfPrograms() {
    return all_.size();
  }

  bool VisitAllPrograms(ProgramVisitor* vis) {
    PRT_ASSERT(vis);
    for(const auto& p : all_) {
      if(!vis->VisitProgram(p))
        return false;
    }
    return true;
  }
}