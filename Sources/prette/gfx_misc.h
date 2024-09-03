#ifndef PRT_GFX_H
#error "Please #include <prette/gfx.h> instead."
#endif //PRT_GFX_H

#ifndef PRT_GFX_MISC_H
#define PRT_GFX_MISC_H

#include "prette/gfx_error.h"

namespace prt {
  enum GlObjectUsage {
    kDynamicUsage = GL_DYNAMIC_DRAW,
    kStaticUsage = GL_STATIC_DRAW,
    kStreamUsage = GL_STREAM_DRAW,
    kDefaultUsage = kDynamicUsage,
  };

  static inline std::ostream&
  operator<<(std::ostream& stream, const GlObjectUsage& rhs) {
    switch(rhs) {
      case kDynamicUsage:
        return stream << "Dynamic";
      case kStaticUsage:
        return stream << "Static";
      default:
        return stream << "<unknown GlObjectUsage: " << rhs << ">";
    }
  }

  namespace gfx {
    void Clear(const GLbitfield mask);

    static inline void
    ClearColorBuffer() {
      return Clear(GL_COLOR_BUFFER_BIT);
    }

    static inline void
    ClearDepthBuffer() {
      return Clear(GL_DEPTH_BUFFER_BIT);
    }

    static inline void
    ClearColorAndDepthBuffer() {
      return Clear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    }

    static inline int
    GetContextFlags() {
      int flags;
      glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
      CHECK_GL(FATAL);
      return flags;
    }

    struct Resource {
      Resource() = default;
      virtual ~Resource() = default;
      virtual void Bind() const = 0;
      virtual void Unbind() const = 0;
    };

    template<const GLenum Capability, const bool Inverted = false>
    class CapabilityScope {
    public:
      CapabilityScope() {
        if(Inverted) {
          glDisable(Capability);
        } else {
          glEnable(Capability);
        }
        CHECK_GL(FATAL);
      }
      virtual ~CapabilityScope() {
        if(Inverted) {
          glEnable(Capability);
        } else {
          glDisable(Capability);
        }
        CHECK_GL(FATAL);
      }

      bool IsInverted() const {
        return Inverted;
      }
    };
    
    enum GlFunction : GLenum {
      kNever = GL_NEVER,
      kLess = GL_LESS,
      kEqual = GL_EQUAL,
      kLequal = GL_LEQUAL,
      kGreater = GL_GREATER,
      kNotEqual = GL_NOTEQUAL,
      kGequal = GL_GEQUAL,
      kAlways = GL_ALWAYS
    };
    typedef GlFunction DepthFunction;

    static constexpr const DepthFunction kDefaultDepthFunction = kLess;

    static inline DepthFunction GetCurrentDepthFunction() {
      GLint value;
      glGetIntegerv(GL_DEPTH_FUNC, &value);
      CHECK_GL(FATAL);
      return static_cast<DepthFunction>(value);
    }
    
    template<const bool Inverted = false>
    class DepthTestCapabilityScope : public CapabilityScope<GL_DEPTH_TEST, Inverted> {
    private:
      DepthFunction previous_;
      bool restore_;
    public:
      DepthTestCapabilityScope(const DepthFunction func = kDefaultDepthFunction, const bool restore = true):
        CapabilityScope<GL_DEPTH_TEST, Inverted>(),
        previous_(GetCurrentDepthFunction()),
        restore_(restore) {
        glDepthFunc(func);
        CHECK_GL(FATAL);
      }
      ~DepthTestCapabilityScope() override {
        if(restore_) {
          glDepthFunc(previous_);
          CHECK_GL(FATAL);
        }
      }
    };
    typedef DepthTestCapabilityScope<false> DepthTestScope;
    typedef DepthTestCapabilityScope<true> InvertedDepthTestScope;

    template<const bool Inverted = false>
    class ScissorTestCapabilityScope : public CapabilityScope<GL_SCISSOR_TEST, Inverted> {
    public:
      ScissorTestCapabilityScope():
        CapabilityScope<GL_SCISSOR_TEST, Inverted>() {
      }
      ~ScissorTestCapabilityScope() override = default;
    };
    typedef ScissorTestCapabilityScope<false> ScissorTestScope;
    typedef ScissorTestCapabilityScope<true> InvertedScissorTestScope;
  }

  using gfx::DepthTestScope;
  using gfx::InvertedDepthTestScope;

  using gfx::ScissorTestScope;
  using gfx::InvertedScissorTestScope;

#define DEFINE_RESOURCE_SCOPE(Resource) \
  typedef gfx::BindScope<Resource> Resource##Scope;
}

#endif //PRT_GFX_MISC_H