#ifndef PRT_PROGRAM_H
#define PRT_PROGRAM_H

#include "prette/uri.h"
#include "prette/gfx_object.h"
#include "prette/shader/shader_id.h"
#include "prette/program/program_id.h"
#include "prette/program/program_events.h"
#include "prette/program/program_uniform.h"
#include "prette/program/program_attribute.h"

namespace prt {
  namespace program {
    class Program;
    class ProgramVisitor {
      DEFINE_NON_COPYABLE_TYPE(ProgramVisitor);
    protected:
      ProgramVisitor() = default;
    public:
      virtual ~ProgramVisitor() = default;
      virtual auto VisitProgram(Program* p) -> bool = 0;
    };
  }
  using program::Program;

#define FOR_EACH_PROGRAM_PROPERTY(V)                              \
  V(AttachedShaders, GL_ATTACHED_SHADERS)                         \
  V(ActiveAttributes, GL_ACTIVE_ATTRIBUTES)                       \
  V(ActiveAttributesMaxLength, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH)    \
  V(ActiveUniforms, GL_ACTIVE_UNIFORMS)                           \
  V(ActiveUniformsMaxLength, GL_ACTIVE_UNIFORM_MAX_LENGTH)

  namespace program {
    auto OnProgramEvent() -> rx::observable<ProgramEvent*>;

    static inline auto
    OnProgramEvent(const ProgramId id) -> rx::observable<ProgramEvent*> {
      return OnProgramEvent()
        .filter([id](ProgramEvent* event) {
          //TODO: cleanup
          return event
              && event->GetProgramId() == id;
        });
    }

  #define DEFINE_ON_PROGRAM_EVENT(Name)                               \
    static inline auto                                                \
    On##Name##Event() -> Name##EventObservable {                      \
      return OnProgramEvent()                                         \
        .filter(Name##Event::Filter)                                  \
        .map(Name##Event::Cast);                                      \
    }                                                                 \
    static inline auto                                                \
    On##Name##Event(const ProgramId id) -> Name##EventObservable {    \
      return OnProgramEvent()                                         \
        .filter(Name##Event::FilterBy(id))                            \
        .map(Name##Event::Cast);                                      \
    }
    FOR_EACH_PROGRAM_EVENT(DEFINE_ON_PROGRAM_EVENT)
#undef DEFINE_ON_PROGRAM_EVENT

    class Program : public gfx::Object<ProgramId>,
                    public ProgramEventSource {
      friend class ProgramLinker;
      friend class ProgramBuilder;
      friend class ApplyProgramScope;
      friend class ProgramLinkScope;
      friend class UniformIterator;
      friend class AttributeIterator;
      DEFINE_NON_COPYABLE_TYPE(Program);
    public:
      struct IdComparator {
        auto operator()(Program* lhs, Program* rhs) const -> bool {
          return lhs->GetId() < rhs->GetId();
        }
      };
    protected:
      enum Property : GLenum {
  #define DEFINE_PROGRAM_PROPERTY(Name, GlValue) k##Name = (GlValue),
        FOR_EACH_PROGRAM_PROPERTY(DEFINE_PROGRAM_PROPERTY)
  #undef DEFINE_PROGRAM_PROPERTY
      };
      
      static void UseProgram(const ProgramId id);
      static void DeleteProgram(const ProgramId id);
      static void Attach(const ProgramId program, const ShaderId shader);
      static void Detach(const ProgramId program, const ShaderId shader);

      static inline void
      UseDefault() {
        return UseProgram(kInvalidProgramId);
      }
    protected:
      explicit Program(const Metadata& meta, const ProgramId id);

      auto GetProgramiv(const Property property) const -> int;

      inline auto GetActiveAttributesMaxLength() const -> int {
        return GetProgramiv(kActiveAttributesMaxLength);
      }

      inline auto GetActiveUniformsMaxLength() const -> int {
        return GetProgramiv(kActiveUniformsMaxLength);
      }

      void Publish(ProgramEvent* event) override;
    public:
      ~Program() override;

      virtual auto GetNumberOfAttachedShaders() const -> int {
        return GetProgramiv(kAttachedShaders);
      }

      virtual auto GetNumberOfActiveAttributes() const -> int {
        return GetProgramiv(kActiveAttributes);
      }

      virtual auto GetActiveAttributes() const -> rx::observable<ProgramAttribute>;

      virtual auto GetNumberOfActiveUniforms() const -> int {
        return GetProgramiv(kActiveUniforms);
      }

      virtual auto GetActiveUniforms() const -> rx::observable<ProgramUniform>;

      virtual auto GetUniformLocation(const std::string& name) const -> GLint {
        return glGetUniformLocation(GetId(), name.c_str());
      }

      virtual auto GetUniformBlockIndex(const std::string& name) const -> GLuint {
        return glGetUniformBlockIndex(GetId(), name.c_str());
      }

      virtual void SetUniformBlock(const std::string& name, const GLuint binding) const {
        glUniformBlockBinding(id_, GetUniformBlockIndex(name), binding);
        CHECK_GL;
      }

      auto Accept(ProgramVisitor* vis) -> bool;
      auto VisitAllUniforms(UniformVisitor* vis) const -> bool;
      auto VisitAllAttributes(AttributeVisitor* vis) const -> bool;
      auto ToString() const -> std::string override;
      auto OnEvent() const -> ProgramEventObservable override;
    private:
      static inline auto
      New(const Metadata& meta, const ProgramId id) -> Program* {
        PRT_ASSERT(id);
        return new Program(meta, id);
      }
    public:
      static inline auto
      New(const ProgramId id) -> Program* {
        PRT_ASSERT(id);
        Metadata meta;
        return New(meta, id);
      }

      static auto FromJson(const uri::Uri& uri) -> Program*;

      static inline auto
      FromJson(const uri::basic_uri& uri) -> Program* {
        return FromJson(uri::Uri(uri)); //TODO: sanitize uri -> uri::Uri("file://{0:s}/{1:s}.json", programs_dir, uri.path)
      }
    };

    using ProgramSet = std::set<Program *, Program::IdComparator>;

    auto GetAllPrograms() -> const ProgramSet&;
    auto GetTotalNumberOfPrograms() -> uword;
    auto VisitAllPrograms(ProgramVisitor* vis) -> bool;
  }
}

#endif //PRT_PROGRAM_H