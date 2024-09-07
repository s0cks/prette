#include "prette/program/program.h"

#include "prette/buffer.h"
#include "prette/json_schema.h"

#include "prette/shader/shader_code.h"
#include "prette/shader/shader_compiler.h"

#include "prette/program/program_json.h"
#include "prette/program/program_builder.h"

namespace prt::program {
  static ProgramSet all_; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
  static rx::subject<ProgramEvent*> events_; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

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

  auto OnProgramEvent() -> rx::observable<ProgramEvent*> {
    return events_.get_observable();
  }

  void Program::Publish(ProgramEvent* event) {
    PRT_ASSERT(event);
    const auto& subscriber = events_.get_subscriber();
    subscriber.on_next(event);
  }

  void Program::UseProgram(const ProgramId id) {
    PRT_ASSERT(id);
    glUseProgram(id);
    CHECK_GL;
  }

  void Program::DeleteProgram(const ProgramId id) {
    PRT_ASSERT(id);
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

  auto Program::GetProgramiv(const Property property) const -> int {
    GLint value = 0;
    glGetProgramiv(GetId(), property, &value);
    return static_cast<int>(value);
  }

  auto Program::VisitAllAttributes(AttributeVisitor* vis) const -> bool {
    AttributeIterator iter(this);
    while(iter.HasNext()) {
      const auto next = iter.Next();
      if(!vis->VisitAttribute(next))
        return false;
    }
    return true;
  }

  auto Program::VisitAllUniforms(UniformVisitor* vis) const -> bool {
    UniformIterator iter(this);
    while(iter.HasNext()) {
      const auto next = iter.Next();
      if(!vis->VisitUniform(next))
        return false;
    }
    return true;
  }

  auto Program::GetActiveAttributes() const -> rx::observable<ProgramAttribute> {
    return rx::observable<>::create<ProgramAttribute>([this](rx::subscriber<ProgramAttribute> s) {
      AttributeIterator iter(this);
      while(iter.HasNext()) {
        const auto next = iter.Next();
        s.on_next(next);
      }
      s.on_completed();
    });
  }

  auto Program::GetActiveUniforms() const -> rx::observable<ProgramUniform> {
    return rx::observable<>::create<ProgramUniform>([this](rx::subscriber<ProgramUniform> s) {
      UniformIterator iter(this);
      while(iter.HasNext()) {
        const auto next = iter.Next();
        s.on_next(next);
      }
      s.on_completed();
    });
  }

  auto Program::ToString() const -> std::string {
    std::stringstream ss;
    ss << "Program(";
    ss << "meta=" << GetMeta() << ", ";
    ss << "id=" << GetId();
    ss << ")";
    return ss.str();
  }

  auto Program::Accept(ProgramVisitor* vis) -> bool {
    PRT_ASSERT(vis);
    return vis->VisitProgram(this);
  }

  auto Program::OnEvent() const -> ProgramEventObservable {
    return events_.get_observable();
  }

  static inline auto
  GetProgramNameFromDirectory(const std::string& dir) -> std::string {
    const auto slashpos = dir.find_last_of('/');
    if(slashpos == std::string::npos)
      return {};
    return {};
  }

  auto Program::FromJson(const uri::Uri& uri) -> Program* {
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
    for(const auto& shader : shaders)
      builder.Attach(shader.GetShader());
    return builder.Build();
  }

  auto GetAllPrograms() -> const ProgramSet& {
    return all_;
  }
  
  auto GetTotalNumberOfPrograms() -> uword {
    return all_.size();
  }

  auto VisitAllPrograms(ProgramVisitor* vis) -> bool {
    PRT_ASSERT(vis);
    for(const auto& p : all_) {
      if(!vis->VisitProgram(p))
        return false;
    }
    return true;
  }
}