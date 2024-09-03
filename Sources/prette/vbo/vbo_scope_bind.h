#ifndef PRT_VBO_SCOPE_H
#error "Please #include <prette/vbo/vbo_scope.h> instead."
#endif //PRT_VBO_SCOPE_H

#ifndef PRT_VBO_SCOPE_BIND_H
#define PRT_VBO_SCOPE_BIND_H

#include "prette/vbo/vbo_scope.h"
#include "prette/gfx_bind_scope.h"

namespace prt::vbo {
  class VboBindScope : public VboScope {
  public:
    explicit VboBindScope(Vbo* vbo):
      VboScope(vbo) {
      Bind();
    }
    ~VboBindScope() override {
      Unbind();
    }

    void Bind() const;
    void Unbind() const;
  };
}

#endif //PRT_VBO_SCOPE_BIND_H