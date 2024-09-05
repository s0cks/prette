#include "prette/vbo/vbo_attr_binder.h"

namespace prt::vbo {
  VboAttributeBinder::VboAttributeBinder(const uword stride):
    AttributeVisitor(),
    stride_(stride),
    offset_(0) {
  }

  VboAttributeBinder::~VboAttributeBinder() {
    // do something?
  }

  void VboAttributeBinder::Bind(Attribute* attr) {
    PRT_ASSERT(attr);
    glVertexAttribPointer(attr->GetIndex(), attr->GetSize(), attr->GetType()->GetGlType(), attr->IsNormalized(), GetStride(), (const GLvoid*) offset_);
    CHECK_GL;
  }

  void VboAttributeBinder::Enable(Attribute* attr) {
    PRT_ASSERT(attr);
    glEnableVertexAttribArray(attr->GetIndex());
    CHECK_GL;
  }

  bool VboAttributeBinder::VisitAttribute(Attribute* attr) {
    PRT_ASSERT(attr);
    DLOG(INFO) << "binding " << attr->ToString() << ".....";
    Bind(attr);
    Enable(attr);
    offset_ += attr->GetAllocationSize();
    return true;
  }
}