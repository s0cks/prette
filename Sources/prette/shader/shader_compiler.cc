#include "prette/shader/shader_compiler.h"

#include "prette/thread_local.h"
#include "prette/pretty_logger.h"
#include "prette/shader/shader_unit_printer.h"
#include "prette/shader/shader_source_attacher.h"

namespace prt::shader {
  static inline void
  Compile(const ShaderId id) {
    glCompileShader(id);
    CHECK_GL;
  }

  static inline void
  AttachAndCompile(const ShaderId id, ShaderUnit* unit) {
    PRT_ASSERT(IsValidShaderId(id));
    PRT_ASSERT(unit);
    ShaderSourceAttacher attacher(unit);
    if(!attacher.AttachSources(id)) {
      DLOG(ERROR) << "failed to attach sources.";
      return;
    }
    Compile(id);
  }

#ifdef PRT_DEBUG
#define __ google::LogMessage(GetFile(), GetLine(), GetSeverity()).stream() << GetIndentString()
  class ShaderCompilerStatsPrinter : public PrettyLogger {
    DEFINE_NON_COPYABLE_TYPE(ShaderCompilerStatsPrinter);
  public:
    ShaderCompilerStatsPrinter(const google::LogSeverity severity,
                               const char* file,
                               int line,
                               const int indent = 0):
      PrettyLogger(severity, file, line, indent) {
    }
    explicit ShaderCompilerStatsPrinter(PrettyLogger* parent):
      PrettyLogger(parent) {
    }
    ~ShaderCompilerStatsPrinter() override = default;

    void PrintStats(ShaderCompiler* compiler) {
      using units::time::nanosecond_t;
      PRT_ASSERT(compiler);
      __ << "Shader Compiler Stats:";
      Indent();
      {
        __ << "Total Compiled: " << compiler->GetCompiled();
        __ << "Duration: ";
        Indent();
        {
          // NOLINTBEGIN(cppcoreguidelines-narrowing-conversions)
          const auto& duration = compiler->GetDurationSeries();
          __ << "Avg: " << nanosecond_t(duration.average());
          __ << "Min: " << nanosecond_t(duration.min());
          __ << "Max: " << nanosecond_t(duration.max());
          // NOLINTEND(cppcoreguidelines-narrowing-conversions)
        }
        Deindent();
      }
      Deindent();
    }
  public:
    static inline void
    Print(ShaderCompiler* compiler, google::LogSeverity severity, const char* file, const int line, const int indent = 0) {
      ShaderCompilerStatsPrinter printer(severity, file, line, 0);
      printer.PrintStats(compiler);
    }
  };
#undef __

  void ShaderCompiler::PrintStats(const google::LogSeverity severity, const char* file, const int line) {
    ShaderCompilerStatsPrinter::Print(this, severity, file, line);
  }

#define __ google::LogMessage(file, line, severity).stream()
  void ShaderCompiler::PrintCompilationStatus(ShaderUnit* unit, const ShaderCompileStatus& status, const google::LogSeverity severity, const char *file, const int line) {
    PRT_ASSERT(unit);
    using units::time::nanosecond_t;
    const auto duration = duration_.first();
    __ << "Id: " << status.GetShaderId();
    const auto& meta = unit->GetMeta();
    __ << "Status: " << status;
    __ << "Duration: " << nanosecond_t(duration); // NOLINT(cppcoreguidelines-narrowing-conversions)
    ShaderUnitPrinter::Print<>(unit, file, line);
  }
#undef __

#endif //PRT_DEBUG

  auto ShaderCompiler::CompileShaderUnit(ShaderUnit* unit) -> ShaderId {
    if(!unit)
      return kInvalidShaderId;
    using namespace units::time;
    VLOG(1) << "compiling " << unit << "....";
    const auto start_ns = uv_hrtime();
    const auto id = glCreateShader(unit->GetType());
    CHECK_GL;
    if(IsValidShaderId(id))
      AttachAndCompile(id, unit);
    const auto stop_ns = uv_hrtime();
    const auto total_ns = (stop_ns - start_ns);
    duration_.Append(total_ns);
    compiled_ += 1;
    const auto status = ShaderCompileStatus(id);
#ifdef PRT_DEBUG
    const auto severity = status ? google::INFO : google::ERROR;
    PrintCompilationStatus(unit, status, severity, __FILE__, __LINE__);
    PrintStats(severity, __FILE__, __LINE__);
#else
    VLOG(1) << unit << " compilation finished in " << nanosecond_t(total_ns); // NOLINT(cppcoreguidelines-narrowing-conversions)
#endif //PRT_DEBUG
    if(!status)
      return kInvalidShaderId; //TODO: cleanup shader id
    //TODO: implement: ShaderCompilerEventSource::Publish<ShaderCompiledEvent>(nullptr);
    return id;
  }

  static LazyThreadLocal<ShaderCompiler> compiler_; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

  static inline auto
  HasCompiler() -> bool {
    return (bool) compiler_;
  }

  auto ShaderCompiler::GetCompiler() -> ShaderCompiler* {
    const auto compiler = compiler_.Get();
    PRT_ASSERT(compiler);
    return compiler;
  }

  auto ShaderCompiler::Compile(ShaderUnit* unit) -> ShaderId {
    PRT_ASSERT(unit);
    return GetCompiler()->CompileShaderUnit(unit);
  }

  auto ShaderCompiler::CompileAsync(ShaderUnit* unit) -> rx::observable<ShaderId> {
    if(!unit)
      return rx::observable<>::empty<ShaderId>();
    return rx::observable<>::create<ShaderId>([unit](rx::subscriber<ShaderId> s) {
      const auto id = GetCompiler()->CompileShaderUnit(unit);
      s.on_next(id);
      s.on_completed();
    });
  }
}