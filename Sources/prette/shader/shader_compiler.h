#ifndef PRT_SHADER_COMPILER_H
#define PRT_SHADER_COMPILER_H

#include <fmt/format.h>

#include "prette/rx.h"
#include "prette/series.h"
#include "prette/counter.h"
#include "prette/shader/shader.h"
#include "prette/shader/shader_code.h"
#include "prette/shader/shader_unit.h"

namespace prt::shader {
  typedef rx::observable<ShaderCompilerEvent*> ShaderCompilerEventObservable;
  typedef rx::subject<ShaderCompilerEvent*> ShaderCompilerEventSubject;

  class ShaderCompilerEventSource {
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
    virtual ShaderCompilerEventObservable OnEvent() const = 0;

    inline rx::observable<ShaderCompiledEvent*> OnCompiled() const {
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
    typedef TimeSeries<10> DurationSeries;
    typedef Counter<uint64_t> CompiledCounter;
  protected:
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
    virtual ~ShaderCompiler() = default;
    virtual ShaderId CompileShaderUnit(ShaderUnit* unit);

    const DurationSeries& GetDurationSeries() const {
      return duration_;
    }

    const CompiledCounter& GetCompiled() const {
      return compiled_;
    }
    
    ShaderCompilerEventObservable OnEvent() const override {
      return events_.get_observable();
    }
  private:
    static inline uri::Uri
    FormatBasicUri(const uri::basic_uri& uri) {
      if(!(StartsWith(uri, "shader:") || StartsWith(uri, "file:"))) {
        if(StartsWith(uri, "/"))
          return uri::Uri(fmt::format("file://{0:s}", uri));
        return uri::Uri(fmt::format("shader:{0:s}", uri));
      }
      return uri::Uri(uri);
    }
  public:
    static ShaderCompiler* GetCompiler();
    static ShaderId Compile(ShaderUnit* unit);
    static rx::observable<ShaderId> CompileAsync(ShaderUnit* unit);
  };
}

#endif //PRT_SHADER_COMPILER_H