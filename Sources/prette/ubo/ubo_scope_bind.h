#ifndef PRT_UBO_SCOPE_H
#error "Please #include <prette/ubo/ubo_scope.h> instead."
#endif //PRT_UBO_SCOPE_H

#ifndef PRT_UBO_SCOPE_BIND_H
#define PRT_UBO_SCOPE_BIND_H

#include "prette/gfx_bind_scope.h"
#include "prette/ubo/ubo_scope.h"

namespace prt::ubo {
  class UboBindScope : public UboScope,
                       public gfx::BindScope {
  public:
    explicit UboBindScope(Ubo* ubo):
      UboScope(ubo) {
      Bind();
    }
    ~UboBindScope() override {
      Unbind();
    }

    void Bind() const override;
    void Unbind() const override;
  };
}

#endif //PRT_UBO_SCOPE_BIND_H