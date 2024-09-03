#ifndef PRT_PROGRAM_H
#define PRT_PROGRAM_H

#include "prette/rx.h"
#include "prette/reference.h"
#include "prette/gfx_object.h"
#include "prette/resource/resource.h"

#include "prette/shader/shader_id.h"
#include "prette/program/program_id.h"
#include "prette/program/program_events.h"
#include "prette/program/program_uniform.h"
#include "prette/program/program_attribute.h"

namespace prt {
  namespace program {
    class Program;
    class ProgramVisitor {
    protected:
      ProgramVisitor() = default;
    public:
      virtual ~ProgramVisitor() = default;
      virtual bool VisitProgram(Program* p) = 0;
    };
  }
  using program::Program;

  namespace resource {
    typedef Reference<Program> ProgramRef;
  }
  using res::ProgramRef;

#define FOR_EACH_PROGRAM_PROPERTY(V)                              \
  V(AttachedShaders, GL_ATTACHED_SHADERS)                         \
  V(ActiveAttributes, GL_ACTIVE_ATTRIBUTES)                       \
  V(ActiveAttributesMaxLength, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH)    \
  V(ActiveUniforms, GL_ACTIVE_UNIFORMS)                           \
  V(ActiveUniformsMaxLength, GL_ACTIVE_UNIFORM_MAX_LENGTH)

  namespace program {
    rx::observable<ProgramEvent*> OnProgramEvent();

    static inline rx::observable<ProgramEvent*>
    OnProgramEvent(const ProgramId id) {
      return OnProgramEvent()
        .filter([id](ProgramEvent* event) {
          //TODO: cleanup
          return event
              && event->GetProgramId() == id;
        });
    }

  #define DEFINE_ON_PROGRAM_EVENT(Name)                           \
    static inline rx::observable<Name##Event*>                    \
    On##Name##Event() {                                           \
      return OnProgramEvent()                                     \
        .filter(Name##Event::Filter)                              \
        .map(Name##Event::Cast);                                  \
    }                                                             \
    static inline rx::observable<Name##Event*>                    \
    On##Name##Event(const ProgramId id) {                         \
      return OnProgramEvent()                                     \
        .filter(Name##Event::FilterBy(id))                        \
        .map(Name##Event::Cast);                                  \
    }
    FOR_EACH_PROGRAM_EVENT(DEFINE_ON_PROGRAM_EVENT)
#undef DEFINE_ON_PROGRAM_EVENT

    class Program : public gfx::ObjectTemplate<ProgramId>,
                    public ProgramEventSource {
      friend class ProgramLinker;
      friend class ProgramBuilder;
      friend class ApplyProgramScope;
      friend class ProgramLinkScope;

      friend class UniformIterator;
      friend class AttributeIterator;
    public:
      struct IdComparator {
        bool operator()(Program* lhs, Program* rhs) const {
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

      int GetProgramiv(const Property property) const;

      inline int GetActiveAttributesMaxLength() const {
        return GetProgramiv(kActiveAttributesMaxLength);
      }

      inline int GetActiveUniformsMaxLength() const {
        return GetProgramiv(kActiveUniformsMaxLength);
      }

      void Publish(ProgramEvent* event) override;
    public:
      ~Program() override;

      virtual int GetNumberOfAttachedShaders() const {
        return GetProgramiv(kAttachedShaders);
      }

      virtual int GetNumberOfActiveAttributes() const {
        return GetProgramiv(kActiveAttributes);
      }

      virtual rx::observable<ProgramAttribute> GetActiveAttributes() const;

      virtual int GetNumberOfActiveUniforms() const {
        return GetProgramiv(kActiveUniforms);
      }

      virtual rx::observable<ProgramUniform> GetActiveUniforms() const;

      virtual GLint GetUniformLocation(const std::string& name) const {
        return glGetUniformLocation(GetId(), name.c_str());
      }

      virtual GLint GetUniformBlockIndex(const std::string& name) const {
        return glGetUniformBlockIndex(GetId(), name.c_str());
      }

      virtual void SetUniformBlock(const std::string& name, const GLuint binding) const {
        glUniformBlockBinding(id_, GetUniformBlockIndex(name), binding);
        CHECK_GL(FATAL);
      }

      bool Accept(ProgramVisitor* vis);
      bool VisitAllUniforms(UniformVisitor* vis) const;
      bool VisitAllAttributes(AttributeVisitor* vis) const;
      std::string ToString() const override;
      ProgramEventObservable OnEvent() const override;
    private:
      static inline Program*
      New(const Metadata& meta, const ProgramId id) {
        PRT_ASSERT(IsValidProgramId(id));
        return new Program(meta, id);
      }
    public:
      static inline Program*
      New(const ProgramId id) {
        PRT_ASSERT(IsValidProgramId(id));
        Metadata meta;
        return New(meta, id);
      }

      static Program* FromJson(const uri::Uri& uri);

      static inline Program*
      FromJson(const uri::basic_uri& uri) {
        return FromJson(uri::Uri(uri)); //TODO: sanitize uri -> uri::Uri("file://{0:s}/{1:s}.json", programs_dir, uri.path)
      }
    };

    typedef std::set<Program*, Program::IdComparator> ProgramSet;

    const ProgramSet& GetAllPrograms();
    uword GetTotalNumberOfPrograms();
    bool VisitAllPrograms(ProgramVisitor* vis);
  }
}

#endif //PRT_PROGRAM_H