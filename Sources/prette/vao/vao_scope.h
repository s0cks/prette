#ifndef PRT_VAO_SCOPE_H
#define PRT_VAO_SCOPE_H

#include "prette/vao/vao_id.h"
#include "prette/gfx_bind_scope.h"

namespace prt {
  namespace vao {
    class Vao;
    class VaoScope {
    protected:
      Vao* vao_;

      explicit VaoScope(Vao* vao):
        vao_(vao) {
      } 
    public:
      virtual ~VaoScope() = default;

      Vao* GetVao() const {
        return vao_;
      }

      VaoId GetVaoId() const;
    };
  }
}

#include "prette/vao/vao_scope_bind.h"

#endif //PRT_VAO_SCOPE_H