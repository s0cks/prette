#ifndef PRT_CULL_FACE_SCOPE_H
#define PRT_CULL_FACE_SCOPE_H

#include "prette/gfx.h"

namespace prt {
  namespace gfx {
#define FOR_EACH_CULL_FACE_MODE(V)          \
  V(Clockwise,          GL_CW)              \
  V(CounterClockwise,   GL_CCW)

#define FOR_EACH_CULL_FACE_FACE(V)          \
  V(Front,              GL_FRONT)           \
  V(Back,               GL_BACK)            \
  V(FrontAndBack,       GL_FRONT_AND_BACK)

    template<const bool Inverted = false>
    class CullFaceCapabilityScope : public CapabilityScope<GL_CULL_FACE, Inverted> {
    public:
      enum Mode : GLenum {
#define DEFINE_MODE(Name, GlValue) k##Name##Mode = (GlValue),
        FOR_EACH_CULL_FACE_MODE(DEFINE_MODE)
#undef DEFINE_MODE
        kTotalNumberOfModes,
        kDefaultMode = kCounterClockwiseMode,
      };

      friend auto operator<<(std::ostream& stream, const Mode& rhs) -> std::ostream& {
        switch(rhs) {
#define DEFINE_TO_STRING(Name, GlValue)                                                                 \
          case k##Name##Mode: return stream << #Name << " (" << #GlValue << ":" << (GlValue) << ")";
          FOR_EACH_CULL_FACE_MODE(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
          default: return stream << "Unknown Mode";
        }
      }

      static inline auto
      GetCurrentMode() -> Mode {
        GLint mode{};
        glGetIntegerv(GL_CULL_FACE_MODE, &mode);
        CHECK_GL;
        return static_cast<Mode>(mode);
      }

      static inline void
      SetMode(const Mode& rhs) {
        glFrontFace(rhs);
        CHECK_GL;
      }

      enum Face : GLenum {
#define DEFINE_FACE(Name, GlValue) k##Name##Face = (GlValue),
        FOR_EACH_CULL_FACE_FACE(DEFINE_FACE)
#undef DEFINE_FACE
        kTotalNumberOfFaces,
        kDefaultFace = kBackFace,
      };

      friend auto operator<<(std::ostream& stream, const Face& rhs) -> std::ostream& {
        switch(rhs) {
#define DEFINE_TO_STRING(Name, GlValue)                                                                   \
          case k##Name##Face: return stream << #Name << " (" << #GlValue << ":" << (GlValue) << ")";
          FOR_EACH_CULL_FACE_FACE(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
          default: return stream << "Unknown Face";
        }
      }

      static inline auto
      GetCurrentFace() -> Face {
        GLint face{};
        glGetIntegerv(GL_FRONT_FACE, &face);
        CHECK_GL;
        return static_cast<Face>(face);
      }

      static inline void
      SetFace(const Face& rhs) {
        glCullFace(rhs);
        CHECK_GL;
      }
    private:
      Mode old_mode_;
      Mode new_mode_;
      Face old_face_;
      Face new_face_;
      bool restore_;
    public:
      CullFaceCapabilityScope(const Mode mode = kDefaultMode,
                              const Face face = kDefaultFace,
                              const bool restore = true):
        CapabilityScope<GL_CULL_FACE, Inverted>(),
        old_mode_(restore && mode != kDefaultMode ? GetCurrentMode() : mode),
        new_mode_(mode),
        old_face_(restore && face != kDefaultFace ? GetCurrentFace() : face),
        new_face_(face),
        restore_(restore) {
        if(old_mode_ != new_mode_)
          SetMode(new_mode_);
        if(old_face_ != new_face_)
          SetFace(new_face_);
      }
      ~CullFaceCapabilityScope() override {
        if(restore_) {
          if(new_mode_ != old_mode_)
            SetMode(new_mode_);
          if(new_face_ != old_face_)
            SetFace(new_face_);
        }
      }

      auto GetMode() const -> Mode {
        return new_mode_;
      }

      auto GetPreviousMode() const -> Mode {
        return old_mode_;
      }

      auto GetFace() const -> Face {
        return new_face_;
      }

      auto GetPreviousFace() const -> Face {
        return old_face_;
      }
    };
    using CullFaceScope = CullFaceCapabilityScope<false>;
    using InvertedCullFaceScope = CullFaceCapabilityScope<true>;
  }
  using gfx::CullFaceScope;
  using gfx::InvertedCullFaceScope;
}

#endif //PRT_CULL_FACE_SCOPE_H