#ifndef PRT_FBO_ATTACHMENT_H
#define PRT_FBO_ATTACHMENT_H

#include <vector>
#include "prette/fbo/fbo_target.h"

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

#define FOR_EACH_FBO_ATTACHMENT_TYPE(V)    \
  V(Color)                                 \
  V(Depth)                                 \
  V(Stencil)

  class Attachment;
#define FORWARD_DECLARE(Name) class Name##Attachment;
  FOR_EACH_FBO_ATTACHMENT_TYPE(FORWARD_DECLARE)
#undef FORWARD_DECLARE

  typedef std::vector<Attachment*> AttachmentList;

  class Attachment {
    friend class FboFactory;
  public:
    enum Type : uint8_t {
#define DEFINE_ATTACHMENT(Name) k##Name,
      FOR_EACH_FBO_ATTACHMENT_TYPE(DEFINE_ATTACHMENT)
#undef DEFINE_ATTACHMENT
    };

    friend std::ostream& operator<<(std::ostream& stream, const Type& rhs) {
      switch(rhs) {
#define DEFINE_TO_STRING(Name) \
        case k##Name: return stream << #Name;
        FOR_EACH_FBO_ATTACHMENT_TYPE(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
        default: return stream << "Unknown Attachment Type";
      }
    }

    static inline int
    CompareType(const Type& lhs, const Type& rhs) {
      if(lhs < rhs) {
        return -1;
      } else if(lhs > rhs) {
        return +1;
      }
      return 0;
    }
  protected:
    Attachment() = default;
    virtual void AttachTo(const FboTarget target = kDefaultTarget) = 0;
  public:
    virtual ~Attachment() = default;
    virtual Type GetType() const = 0;
    virtual AttachmentPoint GetAttachmentPoint() const = 0;
    virtual std::string ToString() const = 0;
    virtual const char* GetName() const = 0;

#define DEFINE_TYPE_CHECK(Name)                                                   \
    virtual Name##Attachment* As##Name##Attachment() { return nullptr; }          \
    bool Is##Name##Attachment() { return As##Name##Attachment() != nullptr; }
    FOR_EACH_FBO_ATTACHMENT_TYPE(DEFINE_TYPE_CHECK)
#undef DEFINE_TYPE_CHECK
  };

#define DECLARE_FBO_ATTACHMENT(Name)                                            \
  public:                                                                       \
    std::string ToString() const override;                                      \
    Name##Attachment* As##Name##Attachment() override { return this; }          \
    const char* GetName() const override { return #Name; }                      \
    Type GetType() const override { return Attachment::k##Name; }
}

#include "prette/fbo/fbo_attachment_color.h"
#include "prette/fbo/fbo_attachment_depth.h"
#include "prette/fbo/fbo_attachment_stencil.h"

#endif //PRT_FBO_ATTACHMENT_H