#ifndef PRT_FBO_SCOPE_H
#error "Please #include <prette/fbo/fbo_scope.h> instead."
#endif //PRT_FBO_SCOPE_H

#ifndef PRT_FBO_SCOPE_BIND_H
#define PRT_FBO_SCOPE_BIND_H

#include "prette/fbo/fbo_scope.h"
#include "prette/gfx_bind_scope.h"

namespace prt::fbo {
  class FboBindScope : public FboScope,
                       public gfx::BindScope {
  public:
    explicit FboBindScope(Fbo* fbo):
      FboScope(fbo) {
      Bind();
    }
    ~FboBindScope() override {
      Unbind();
    }

    void Bind() const override;
    void Unbind() const override;
  };
}

#endif //PRT_FBO_SCOPE_BIND_H