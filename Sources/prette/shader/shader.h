#ifndef PRT_SHADER_H
#define PRT_SHADER_H

#include <set>
#include <optional>
#include "prette/uri.h"
#include "prette/reference.h"
#include "prette/resource/resource.h"

#include "prette/shader/shader_id.h"
#include "prette/shader/shader_unit.h"
#include "prette/shader/shader_type.h"
#include "prette/shader/shader_events.h"

namespace prt {
  namespace shader {
    class Shader;
    class ShaderVisitor {
    protected:
      ShaderVisitor() = default;
    public:
      virtual ~ShaderVisitor() = default;
#define DEFINE_VISIT_SHADER(Name, Ext, GlValue)                     \
      virtual bool Visit##Name##Shader(Name##Shader* shader) = 0;
      FOR_EACH_SHADER_TYPE(DEFINE_VISIT_SHADER)
#undef DEFINE_VISIT_SHADER
    };

    const std::set<std::string>& GetValidFileExtensions();
    rx::observable<ShaderEvent*> OnEvent();
    
    static inline rx::observable<ShaderEvent*>
    OnEvent(const ShaderId id) {
      return OnEvent()
        .filter([id](ShaderEvent* event) {
          return event
              && event->GetShaderId() == id;
        });
    }

#define DEFINE_ON_SHADER_EVENT(Name)                              \
    static inline rx::observable<Name##Event*>                    \
    On##Name##Event() {                                           \
      return OnEvent()                                            \
        .filter([](ShaderEvent* event) {                          \
          return event                                            \
              && event->Is##Name##Event();                        \
        })                                                        \
        .map([](ShaderEvent* event) {                             \
          PRT_ASSERT(event);                                      \
          PRT_ASSERT(event->Is##Name##Event());                   \
          return event->As##Name##Event();                        \
        });                                                       \
    }                                                             \
    static inline rx::observable<Name##Event*>                    \
    On##Name##Event(const ShaderId id) {                          \
      return OnEvent()                                            \
        .filter([id](ShaderEvent* event) {                        \
          return event                                            \
              && event->Is##Name##Event()                         \
              && event->GetShaderId() == id;                      \
        })                                                        \
        .map([id](ShaderEvent* event) {                           \
          PRT_ASSERT(event);                                      \
          PRT_ASSERT(event->Is##Name##Event());                   \
          PRT_ASSERT(event->GetShaderId() == id);                 \
          return event->As##Name##Event();                        \
        });                                                       \
    }
    FOR_EACH_SHADER_EVENT(DEFINE_ON_SHADER_EVENT)
#undef DEFINE_ON_SHADER_EVENT

    class ShaderCode;
    class ShaderCompiler;
    class Shader : public Object {
      friend class ShaderCompiler;
      friend class VertexShader;
      friend class FragmentShader;
      friend class GeometryShader;
      friend class TessEvalShader;
      friend class TessControlShader;
    public:
      struct IdComparator {
        bool operator()(Shader* lhs, Shader* rhs) const {
          return lhs->GetId() < rhs->GetId();
        }
      };

      struct Comparator {
        bool operator()(const Shader* lhs,
                        const Shader* rhs) const {
          return lhs->GetId() == rhs->GetId()
              && lhs->GetType() == rhs->GetType();
        }
      };
    private:
      static void Publish(ShaderEvent* event);

      template<typename E, typename... Args>
      static inline void
      Publish(Args... args) {
        E event(args...);
        return Publish(&event);
      }
    protected:
      ShaderId id_;

      Shader(const Metadata& meta, const ShaderId id);
    public:
      virtual ~Shader();
      virtual ShaderType GetType() const = 0;
      virtual std::string ToString() const = 0;
      virtual bool Accept(ShaderVisitor* vis) = 0;
      
      ShaderId GetId() const {
        return id_;
      }

      virtual bool Equals(const Shader* rhs) const {
        return GetType() == rhs->GetType()
            && GetId() == rhs->GetId();
      }

#define DEFINE_TYPE_CHECK(Name, Ext, GlValue)                             \
      virtual Name##Shader* As##Name##Shader() { return nullptr; }        \
      bool Is##Name##Shader() { return As##Name##Shader() != nullptr; }
      FOR_EACH_SHADER_TYPE(DEFINE_TYPE_CHECK)
#undef DEFINE_TYPE_CHECK

      ShaderEventObservable OnEvent() const {
        return shader::OnEvent(GetId());
      }

#define DEFINE_ON_SHADER_EVENT(Name)                    \
      Name##EventObservable On##Name##Event() const {   \
        return shader::On##Name##Event(GetId());        \
      }
      FOR_EACH_SHADER_EVENT(DEFINE_ON_SHADER_EVENT)
#undef DEFINE_ON_SHADER_EVENT
    };

    typedef std::set<Shader*, Shader::IdComparator> ShaderSet;

    template<const ShaderType Type>
    class ShaderTemplate : public Shader {
    protected:
      explicit ShaderTemplate(const Metadata& meta, const ShaderId id):
        Shader(meta, id) {
      }
    public:
      ~ShaderTemplate() override = default;

      ShaderType GetType() const override {
        return Type;
      }
    };

#define DECLARE_SHADER_TYPE(Name)                                   \
    public:                                                         \
      std::string ToString() const override;                        \
      Name##Shader* As##Name##Shader() override { return this; }    \
      bool Accept(ShaderVisitor* vis) override {                    \
        PRT_ASSERT(vis);                                            \
        return vis->Visit##Name##Shader(this);                      \
      }                                                             \
    private:                                                        \
      static inline Name##Shader*                                   \
      New(const Metadata& meta, const ShaderId id) {                \
        return new Name##Shader(meta, id);                          \
      }                                                             \
    public:                                                         \
      static Name##Shader* New(ShaderUnit* unit);                   \
      static const std::set<std::string> kValidExtensions;

    const ShaderSet& GetAllShaders();
    uword GetTotalNumberOfShaders();
    bool VisitAllShaders(ShaderVisitor* vis);
#define DECLARE_VISIT_SHADERS(Name, Ext, GlValue)                   \
    bool VisitAll##Name##Shaders(ShaderVisitor* vis);
    FOR_EACH_SHADER_TYPE(DECLARE_VISIT_SHADERS)
#undef DECLARE_VISIT_SHADERS

#ifdef PRT_DEBUG
    bool PrintAllShaders(const google::LogSeverity severity = google::INFO);

#define DECLARE_PRINT_ALL_SHADERS(Name, Ext, GlValue)                                   \
    bool PrintAll##Name##Shaders(const google::LogSeverity severity = google::INFO);
    FOR_EACH_SHADER_TYPE(DECLARE_PRINT_ALL_SHADERS)
#undef DECLARE_PRINT_ALL_SHADERS
#endif //PRT_DEBUG
  }

  using shader::Shader;
#define DEFINE_USE_SHADER_TYPE(Name, Ext, GlValue)      \
  using shader::Name##Shader;
  FOR_EACH_SHADER_TYPE(DEFINE_USE_SHADER_TYPE)
#undef DEFINE_USE_SHADER_TYPE
}

#include "prette/shader/shader_vertex.h"
#include "prette/shader/shader_fragment.h"
#include "prette/shader/shader_geometry.h"
#include "prette/shader/shader_tess_eval.h"
#include "prette/shader/shader_tess_control.h"

#undef DECLARE_SHADER_TYPE

#endif //PRT_SHADER_H