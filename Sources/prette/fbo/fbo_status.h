#ifndef PRT_FBO_STATUS_H
#define PRT_FBO_STATUS_H

#include "prette/fbo/fbo_id.h"
#include "prette/fbo/fbo_target.h"

namespace prt::fbo {
#define FOR_EACH_FBO_STATUS(V)                                                      \
  V(Complete, GL_FRAMEBUFFER_COMPLETE)                                              \
  V(Undefined, GL_FRAMEBUFFER_UNDEFINED)                                            \
  V(IncompleteAttachment, GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)                     \
  V(IncompleteMissingAttachment, GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)      \
  V(IncompleteDrawBuffer, GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)                    \
  V(IncompleteReadBuffer, GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)                    \
  V(IncompleteMultisample, GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE)                   \
  V(IncompleteLayerTargets, GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS)                \
  V(Unsupported, GL_FRAMEBUFFER_UNSUPPORTED)

  class FboStatus {
    enum Status : GLenum {
#define DEFINE_STATUS(Name, GlValue) k##Name = (GlValue),
      FOR_EACH_FBO_STATUS(DEFINE_STATUS)
#undef DEFINE_STATUS
    };

    static inline constexpr const char* ToString(const Status& rhs) {
      switch(rhs) {
#define DEFINE_TO_STRING(Name, GlValue) \
        case Status::k##Name: return #Name;
        FOR_EACH_FBO_STATUS(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
        default: return "unknown";
      }
    }

    friend std::ostream& operator<<(std::ostream& stream, const Status& rhs) {
      return stream << ToString(rhs);
    }
  protected:
    FboTarget target_;
    Status status_;
  public:
    explicit FboStatus(const FboTarget target = kDefaultTarget):
      target_(target),
      status_(static_cast<Status>(glCheckFramebufferStatus(target))) {
    }
    virtual ~FboStatus() = default;

    FboTarget GetTarget() const {
      return target_;
    }

    Status GetStatus() const {
      return status_;
    }

#define DEFINE_TYPE_CHECK(Name, GlValue)            \
    inline bool Is##Name() const {                  \
      return GetStatus() == Status::k##Name;        \
    }
    FOR_EACH_FBO_STATUS(DEFINE_TYPE_CHECK)
#undef DEFINE_TYPE_CHECK

    operator bool () const {
      return IsComplete();
    }

    friend std::ostream& operator<<(std::ostream& stream, const FboStatus& rhs) {
      stream << "FboStatus(";
      stream << "target=" << rhs.GetTarget() << ", ";
      stream << "status=" << rhs.GetStatus();
      stream << ")";
      return stream;
    }
  };
}

#endif //PRT_FBO_STATUS_H