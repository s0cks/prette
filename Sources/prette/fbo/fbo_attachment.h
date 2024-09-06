#ifndef PRT_FBO_ATTACHMENT_H
#define PRT_FBO_ATTACHMENT_H

#include <vector>
#include "prette/fbo/fbo_target.h"
#include "prette/fbo/fbo_attachment_point.h"

namespace prt::fbo {
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
    friend class FboBuilder;
  protected:
    Attachment() = default;
    virtual void AttachTo(const FboTarget target = kDefaultTarget) = 0;
  public:
    virtual ~Attachment() = default;
    virtual AttachmentPoint GetAttachmentPoint() const = 0;
    virtual int CompareTo(Attachment* rhs) const = 0;
    virtual bool Equals(Attachment* rhs) const = 0;
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
    int CompareTo(Attachment* rhs) const override;                              \
    bool Equals(Attachment* rhs) const override;                                \
    const char* GetName() const override { return #Name; }
}

#include "prette/fbo/fbo_attachment_color.h"
#include "prette/fbo/fbo_attachment_depth.h"
#include "prette/fbo/fbo_attachment_stencil.h"

#endif //PRT_FBO_ATTACHMENT_H