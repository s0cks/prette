#include "prette/shader/shader_compiler.h"

#include "prette/thread_local.h"
#include "prette/shader/shader_unit_printer.h"
#include "prette/shader/shader_compile_status.h"

namespace prt::shader {
  class ShaderSourceAttacher : public ShaderCodeVisitor {
    DEFINE_NON_COPYABLE_TYPE(ShaderSourceAttacher);
  private:
    ShaderId target_;
    ShaderUnit* unit_;
    std::vector<GLchar*> sources_;
    std::vector<GLint> lengths_;
    uword num_sources_ = 0;
  public:
    ShaderSourceAttacher(const ShaderId target,
                         ShaderUnit* unit):
      ShaderCodeVisitor(),
      target_(target),
      unit_(unit),
      sources_(),
      lengths_() {
      PRT_ASSERT(unit);
      const auto num_sources = unit->GetSize();
      sources_.reserve(num_sources);
      lengths_.reserve(num_sources);
    }
    ~ShaderSourceAttacher() override = default;

    auto GetTarget() const -> ShaderId {
      return target_;
    }

    auto GetUnit() const -> ShaderUnit* {
      return unit_;
    }

    void Append(ShaderCode* code) {
      PRT_ASSERT(code);
      // NOLINTNEXTLINE
      sources_.push_back((GLchar*) code->GetData());
      lengths_.push_back((GLint) code->GetLength());
      num_sources_++;
    }

    auto VisitShaderCode(ShaderCode* code) -> bool override {
      DLOG(INFO) << "attaching: " << code->ToString();
      Append(code);
      return true;
    }

    auto AttachSources() -> bool {
      if(!GetUnit()->Accept(this)) {
        DLOG(ERROR) << "failed to visit sources of: " << GetUnit()->ToString();
        return false;
      }

      DLOG(INFO) << "sources:";
      for(auto idx = 0; idx < num_sources_; idx++)
        DLOG(INFO) << " -\n" << std::string(sources_[idx], lengths_[idx]);

      // NOLINTNEXTLINE
      glShaderSource(GetTarget(), num_sources_, &sources_[0], &lengths_[0]);
      CHECK_GL;
      return true;
    }
  };

  static inline void
  Compile(const ShaderId id) {
    glCompileShader(id);
    CHECK_GL;
  }

  static inline void
  AttachAndCompile(const ShaderId id, ShaderUnit* unit) {
    PRT_ASSERT(IsValidShaderId(id));
    PRT_ASSERT(unit);
    ShaderSourceAttacher attacher(id, unit);
    if(!attacher.AttachSources()) {
      DLOG(ERROR) << "failed to attach sources.";
      return;
    }
    
    Compile(id);
  }

  auto ShaderCompiler::CompileShaderUnit(ShaderUnit* unit) -> ShaderId {
    if(!unit)
      return kInvalidShaderId;
    using namespace units::time;
#ifdef PRT_DEBUG
    DLOG(INFO) << "compiling:";
    ShaderUnitPrinter::Print<>(unit, __FILE__, __LINE__);
#else
    LOG(INFO) << "compiling " << unit->ToString() << "....";
#endif //PRT_DEBUG
    const auto start_ns = uv_hrtime();
    const auto id = glCreateShader(unit->GetType());
    if(IsValidShaderId(id))
      AttachAndCompile(id, unit);
    const auto stop_ns = uv_hrtime();
    const auto total_ns = (stop_ns - start_ns);
    duration_.Append(total_ns);
    compiled_ += 1;
    // NOLINTNEXTLINE
    DLOG(INFO) << "compilation finished (" << nanosecond_t(total_ns) << ").";
    const auto status = ShaderCompileStatus(id);
#ifdef PRT_DEBUG
    using namespace units::data;
    const auto severity = status ? google::INFO : google::ERROR;
    LOG_AT_LEVEL(severity) << "Shader: " << id;
    LOG_AT_LEVEL(severity) << "Status: " << status;
    LOG_AT_LEVEL(severity) << "Unit: " << unit->ToString();
    LOG_AT_LEVEL(severity) << "Stats:";
    LOG_AT_LEVEL(severity) << " - Total Compiled: " << GetCompiled();
    const auto& duration = GetDurationSeries();
    // NOLINTNEXTLINE
    LOG_AT_LEVEL(severity) << " - Duration: " << nanosecond_t(total_ns) << "; (Avg/Min/Max): " << nanosecond_t(duration_.average()) << ", " << nanosecond_t(duration_.min()) << ", " << nanosecond_t(duration_.min());
#endif //PRT_DEBUG
    if(!status)
      return kInvalidShaderId; //TODO: cleanup shader id
    //TODO: implement: ShaderCompilerEventSource::Publish<ShaderCompiledEvent>(nullptr);
    return id;
  }

  // NOLINTNEXTLINE
  static ThreadLocal<ShaderCompiler> compiler_;

  static inline auto
  HasCompiler() -> bool {
    return (bool) compiler_;
  }

  auto ShaderCompiler::GetCompiler() -> ShaderCompiler* {
    if(HasCompiler())
      return compiler_.Get();
    const auto compiler = new ShaderCompiler();
    compiler_.Set(compiler);
    return compiler;
  }

  auto ShaderCompiler::Compile(ShaderUnit* unit) -> ShaderId {
    PRT_ASSERT(unit);
    const auto compiler = GetCompiler();
    PRT_ASSERT(compiler);
    return compiler->CompileShaderUnit(unit);
  }

  auto ShaderCompiler::CompileAsync(ShaderUnit* unit) -> rx::observable<ShaderId> {
    if(!unit)
      return rx::observable<>::empty<ShaderId>();

    return rx::observable<>::create<ShaderId>([unit](rx::subscriber<ShaderId> s) {
      const auto compiler = GetCompiler();
      if(!compiler)
        return s.on_error(rx::util::make_error_ptr(std::runtime_error("no ShaderCompiler available on thread.")));
      const auto id = compiler->CompileShaderUnit(unit);
      s.on_next(id);
      s.on_completed();
    });
  }
}