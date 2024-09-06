#ifndef PRT_SHADER_H
#define PRT_SHADER_H

#include <set>
#include <optional>
#include "prette/uri.h"
#include "prette/object.h"
#include "prette/shader/shader_id.h"
#include "prette/shader/shader_type.h"
#include "prette/shader/shader_events.h"

namespace prt {
  namespace shader {
    using uri::ExtensionSet;

    class Shader;
    class ShaderVisitor {
      DEFINE_NON_COPYABLE_TYPE(ShaderVisitor);
    protected:
      ShaderVisitor() = default;
    public:
      virtual ~ShaderVisitor() = default;
#define DEFINE_VISIT_SHADER(Name, Ext, GlValue)                             \
      virtual auto Visit##Name##Shader(Name##Shader* shader) -> bool = 0;
      FOR_EACH_SHADER_TYPE(DEFINE_VISIT_SHADER)
#undef DEFINE_VISIT_SHADER
    };

    auto GetValidFileExtensions() -> const ExtensionSet&;
    auto OnEvent() -> ShaderEventObservable;
    
    static inline auto 
    OnEvent(const ShaderId id) -> ShaderEventObservable {
      return OnEvent()
        .filter([id](ShaderEvent* event) {
          return event
              && event->GetShaderId() == id;
        });
    }

#define DEFINE_ON_SHADER_EVENT(Name)                                \
    static inline auto                                              \
    On##Name##Event() -> Name##EventObservable {                    \
      return OnEvent()                                              \
        .filter([](ShaderEvent* event) {                            \
          return event                                              \
              && event->Is##Name##Event();                          \
        })                                                          \
        .map([](ShaderEvent* event) {                               \
          PRT_ASSERT(event);                                        \
          PRT_ASSERT(event->Is##Name##Event());                     \
          return event->As##Name##Event();                          \
        });                                                         \
    }                                                               \
    static inline auto                                              \
    On##Name##Event(const ShaderId id) -> Name##EventObservable {   \
      return OnEvent()                                              \
        .filter([id](ShaderEvent* event) {                          \
          return event                                              \
              && event->Is##Name##Event()                           \
              && event->GetShaderId() == id;                        \
        })                                                          \
        .map([id](ShaderEvent* event) {                             \
          PRT_ASSERT(event);                                        \
          PRT_ASSERT(event->Is##Name##Event());                     \
          PRT_ASSERT(event->GetShaderId() == id);                   \
          return event->As##Name##Event();                          \
        });                                                         \
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
      DEFINE_NON_COPYABLE_TYPE(Shader);
    public:
      // @deprecated
      struct IdComparator {
        auto operator()(Shader* lhs, Shader* rhs) const -> bool {
          return lhs->GetId() < rhs->GetId();
        }
      };

      struct Comparator {
        auto operator()(const Shader* lhs,
                        const Shader* rhs) const -> bool {
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
    private:
      ShaderId id_;
    protected:
      Shader(const Metadata& meta, const ShaderId id);
    public:
      ~Shader() override;
      virtual auto GetType() const -> ShaderType = 0;
      virtual auto Accept(ShaderVisitor* vis) -> bool = 0;
      auto ToString() const -> std::string override = 0;
      
      auto GetId() const -> ShaderId {
        return id_;
      }

      virtual auto Equals(const Shader* rhs) const -> bool {
        return GetType() == rhs->GetType()
            && GetId() == rhs->GetId();
      }

#define DEFINE_TYPE_CHECK(Name, Ext, GlValue)                                     \
      virtual auto As##Name##Shader() -> Name##Shader* { return nullptr; }        \
      auto Is##Name##Shader() -> bool { return As##Name##Shader() != nullptr; }
      FOR_EACH_SHADER_TYPE(DEFINE_TYPE_CHECK)
#undef DEFINE_TYPE_CHECK

      auto OnEvent() const -> ShaderEventObservable {
        return shader::OnEvent(GetId());
      }

#define DEFINE_ON_SHADER_EVENT(Name)                            \
      auto On##Name##Event() const -> Name##EventObservable {   \
        return shader::On##Name##Event(GetId());                \
      }
      FOR_EACH_SHADER_EVENT(DEFINE_ON_SHADER_EVENT)
#undef DEFINE_ON_SHADER_EVENT
    };

    using ShaderSet = std::set<Shader*, Shader::IdComparator>;

    template<const ShaderType Type>
    class ShaderTemplate : public Shader {
      DEFINE_NON_COPYABLE_TYPE(ShaderTemplate<Type>);
    protected:
      explicit ShaderTemplate(const Metadata& meta, const ShaderId id):
        Shader(meta, id) {
      }
    public:
      ~ShaderTemplate() override = default;

      auto GetType() const -> ShaderType override {
        return Type;
      }
    };

#define DECLARE_SHADER_TYPE(Name)                                           \
    public:                                                                 \
      auto ToString() const -> std::string override;                        \
      auto As##Name##Shader() -> Name##Shader* override { return this; }    \
      auto Accept(ShaderVisitor* vis) -> bool override {                    \
        PRT_ASSERT(vis);                                                    \
        return vis->Visit##Name##Shader(this);                              \
      }                                                                     \
    private:                                                                \
      static inline auto                                                    \
      New(const Metadata& meta, const ShaderId id) -> Name##Shader* {       \
        return new Name##Shader(meta, id);                                  \
      }                                                                     \
    public:                                                                 \
      static auto New(ShaderUnit* unit) -> Name##Shader*;                   \
      static const ExtensionSet kValidExtensions;

    auto GetAllShaders() -> const ShaderSet&;
    auto GetTotalNumberOfShaders() -> uword;
    auto VisitAllShaders(ShaderVisitor* vis) -> bool;
#define DECLARE_VISIT_SHADERS(Name, Ext, GlValue)               \
    auto VisitAll##Name##Shaders(ShaderVisitor* vis) -> bool;
    FOR_EACH_SHADER_TYPE(DECLARE_VISIT_SHADERS)
#undef DECLARE_VISIT_SHADERS

#ifdef PRT_DEBUG
    auto PrintAllShaders(const google::LogSeverity severity = google::INFO) -> bool;

#define DECLARE_PRINT_ALL_SHADERS(Name, Ext, GlValue)                                           \
    auto PrintAll##Name##Shaders(const google::LogSeverity severity = google::INFO) -> bool;
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

#endif //PRT_SHADER_H