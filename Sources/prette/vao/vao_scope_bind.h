#ifndef PRT_VAO_SCOPE_H
#error "Please #include <prette/vao/vao_scope.h> instead."
#endif //PRT_VAO_SCOPE_H

#ifndef PRT_VAO_SCOPE_BIND_H
#define PRT_VAO_SCOPE_BIND_H

#include "prette/vao/vao_scope.h"
#include "prette/gfx_bind_scope.h"

namespace prt::vao {
  class VaoBindScope : public VaoScope {
  public:
    explicit VaoBindScope(Vao* vao):
      VaoScope(vao) {
      Bind();
    }
    ~VaoBindScope() override {
      Unbind();
    }

    void Bind() const;
    void Unbind() const;
  };
}

#endif //PRT_VAO_SCOPE_BIND_H