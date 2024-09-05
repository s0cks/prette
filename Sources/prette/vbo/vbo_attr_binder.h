#ifndef PRT_VBO_ATTR_BINDER_H
#define PRT_VBO_ATTR_BINDER_H

#include "prette/vbo/vbo.h"

namespace prt::vbo {
  class VboAttributeBinder : public AttributeVisitor {
  private:
    uword stride_;
    uword offset_;

    void Bind(Attribute* attr);
    void Enable(Attribute* attr);
  public:
    explicit VboAttributeBinder(const uword stride);
    ~VboAttributeBinder() override;

    uword GetStride() const {
      return stride_;
    }

    bool VisitAttribute(Attribute* attr) override;
  public:
    static inline void
    BindAll(Class* cls) {
      PRT_ASSERT(cls);
      const auto stride = cls->GetAllocationSize();
      VboAttributeBinder binder(stride);
      LOG_IF(ERROR, !cls->VisitAllAttributes(&binder)) << "failed to bind all attributes for: " << cls->ToString();
    }
  };
}

#endif //PRT_VBO_ATTR_BINDER_H