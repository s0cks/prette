#ifndef PRT_FBO_ATTACHMENT_POINT_H
#define PRT_FBO_ATTACHMENT_POINT_H

#include "prette/gfx.h"

namespace prt::fbo {
#define FOR_EACH_FBO_COLOR_ATTACHMENT_POINT(V)  \
  V(0,         GL_COLOR_ATTACHMENT0)            \
  V(1,         GL_COLOR_ATTACHMENT1)            \
  V(2,         GL_COLOR_ATTACHMENT2)            \
  V(3,         GL_COLOR_ATTACHMENT3)            \
  V(4,         GL_COLOR_ATTACHMENT4)            \
  V(5,         GL_COLOR_ATTACHMENT5)            \
  V(6,         GL_COLOR_ATTACHMENT6)            \
  V(7,         GL_COLOR_ATTACHMENT7)            \
  V(8,         GL_COLOR_ATTACHMENT8)            \
  V(9,         GL_COLOR_ATTACHMENT9)            \
  V(10,        GL_COLOR_ATTACHMENT10)           \
  V(11,        GL_COLOR_ATTACHMENT11)           \
  V(12,        GL_COLOR_ATTACHMENT12)           \
  V(13,        GL_COLOR_ATTACHMENT13)           \
  V(14,        GL_COLOR_ATTACHMENT14)           \
  V(15,        GL_COLOR_ATTACHMENT15)           \

#define FOR_EACH_FBO_ATTACHMENT_POINT(V)            \
  V(Depth,          GL_DEPTH_ATTACHMENT)            \
  V(Stencil,        GL_STENCIL_ATTACHMENT)          \
  V(DepthStencil,   GL_DEPTH_STENCIL_ATTACHMENT)
  
  enum AttachmentPoint : GLenum {
#define DEFINE_ATTACHMENT_POINT(Name, GlValue) kColor##Name = (GlValue),
  FOR_EACH_FBO_COLOR_ATTACHMENT_POINT(DEFINE_ATTACHMENT_POINT)
#undef DEFINE_ATTACHMENT_POINT

#define DEFINE_ATTACHMENT_POINT(Name, GlValue) k##Name = (GlValue),
    FOR_EACH_FBO_ATTACHMENT_POINT(DEFINE_ATTACHMENT_POINT)
#undef DEFINE_ATTACHMENT_POINT
  };

  static inline std::ostream&
  operator<<(std::ostream& stream, const AttachmentPoint& rhs) {
    switch(rhs) {
#define DEFINE_TO_STRING(Name, GlValue) \
      case kColor##Name: return stream << "Color" << #Name;
      FOR_EACH_FBO_COLOR_ATTACHMENT_POINT(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING

#define DEFINE_TO_STRING(Name, GlValue) \
      case k##Name: return stream << #Name;
      FOR_EACH_FBO_ATTACHMENT_POINT(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
      default: return stream << "Unknown AttachmentPoint";
    }
  }

  static inline bool
  IsColorAttachmentPoint(const AttachmentPoint& rhs) {
    switch(rhs) {
#define DEFINE_TYPE_CHECK(Name, GlValue)    \
      case kColor##Name: return true;
      FOR_EACH_FBO_COLOR_ATTACHMENT_POINT(DEFINE_TYPE_CHECK)
#undef DEFINE_TYPE_CHECK
      default: return false;
    }
  }

  static inline bool
  IsDepthAttachmentPoint(const AttachmentPoint& rhs) {
    return rhs == kDepth;
  }

  static inline bool
  IsStencilAttachmentPoint(const AttachmentPoint& rhs) {
    return rhs == kStencil;
  }

  static inline bool
  IsDepthStencilAttachmentPoint(const AttachmentPoint& rhs) {
    return rhs == kDepthStencil;
  }
}

#endif //PRT_FBO_ATTACHMENT_POINT_H