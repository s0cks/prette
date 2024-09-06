#ifndef PRT_SHADER_COMPILER_H
#define PRT_SHADER_COMPILER_H

#include <fmt/format.h>

#include "prette/rx.h"
#include "prette/common.h"
#include "prette/series.h"
#include "prette/counter.h"
#include "prette/shader/shader_unit.h"
#include "prette/shader/shader_events.h"

namespace prt::shader {
  using ShaderCompilerEventObservable = rx::observable<ShaderCompilerEvent*>;
  using ShaderCompilerEventSubject = rx::subject<ShaderCompilerEvent*>;

  class ShaderCompilerEventSource {
    DEFINE_NON_COPYABLE_TYPE(ShaderCompilerEventSource);
  protected:
    ShaderCompilerEventSource() = default;

    virtual void Publish(ShaderCompilerEvent* event) = 0;

    template<class E, typename... Args>
    inline void Publish(Args... args) {
      E event(args...);
      return Publish((ShaderCompilerEvent*) &event);
    }
  public:
    virtual ~ShaderCompilerEventSource() = default;
    virtual auto OnEvent() const -> ShaderCompilerEventObservable = 0;

    inline auto OnCompiled() const -> rx::observable<ShaderCompiledEvent*> {
      return OnEvent()
        .filter(ShaderCompiledEvent::Filter)
        .map([](ShaderCompilerEvent* event) {
          PRT_ASSERT(event);
          PRT_ASSERT(event->IsShaderCompiledEvent());
          return event->AsShaderCompiledEvent();
        });
    }
  };

  class ShaderCompiler : public ShaderCompilerEventSource {
    DEFINE_NON_COPYABLE_TYPE(ShaderCompiler);
  public:
    static constexpr const auto kDurationSeriesSize = 10;
    using DurationSeries = TimeSeries<kDurationSeriesSize>;
    using CompiledCounter = Counter<uint64_t>;
  private:
    CompiledCounter compiled_;
    DurationSeries duration_;
    ShaderCompilerEventSubject events_;

    void Publish(ShaderCompilerEvent* event) override {
      PRT_ASSERT(event);
      const auto& subscriber = events_.get_subscriber();
      return subscriber.on_next(event);
    }
  public:
    ShaderCompiler() = default;
     ~ShaderCompiler() override = default;
    virtual auto CompileShaderUnit(ShaderUnit* unit) -> ShaderId;

    auto GetDurationSeries() const -> const DurationSeries& {
      return duration_;
    }

    auto GetCompiled() const -> const CompiledCounter& {
      return compiled_;
    }
    
    auto OnEvent() const -> ShaderCompilerEventObservable override {
      return events_.get_observable();
    }
  private:
    static inline auto
    FormatBasicUri(const uri::basic_uri& uri) -> uri::Uri {
      if(!(StartsWith(uri, "shader:") || StartsWith(uri, "file:"))) {
        if(StartsWith(uri, "/"))
          return { fmt::format("file://{0:s}", uri) };
        return { fmt::format("shader:{0:s}", uri) };
      }
      return { uri };
    }
  public:
    static auto GetCompiler() -> ShaderCompiler*;
    static auto Compile(ShaderUnit* unit) -> ShaderId;
    static auto CompileAsync(ShaderUnit* unit) -> rx::observable<ShaderId>;
  };
}

#endif //PRT_SHADER_COMPILER_H