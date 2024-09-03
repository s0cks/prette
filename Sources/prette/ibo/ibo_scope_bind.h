#ifndef PRT_IBO_SCOPE_H
#error "Please #include <prette/ibo/ibo_scope.h> instead."
#endif //PRT_IBO_SCOPE_H

#ifndef PRT_IBO_BIND_SCOPE_H
#define PRT_IBO_BIND_SCOPE_H

#include "prette/ibo/ibo_scope.h"
#include "prette/gfx_bind_scope.h"

namespace prt::ibo {
  class IboBindScope : public IboScope,
                       public gfx::BindScope {
  public:
    explicit IboBindScope(Ibo* ibo):
      IboScope(ibo) {
      Bind(); //TODO: don't call virtual methods in constructor/destructor
    }
    ~IboBindScope() override {
      Unbind();
    }

    void Bind() const override;
    void Unbind() const override;
  };
}

#endif //PRT_IBO_BIND_SCOPE_H