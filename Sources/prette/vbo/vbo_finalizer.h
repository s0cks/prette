#ifndef PRT_VBO_FINALIZER_H
#define PRT_VBO_FINALIZER_H

#include "prette/common.h"
#include "prette/vbo/vbo_visitor.h"

namespace prt::vbo {
  class VboFinalizer : public VboVisitor {
  protected:
    void Finalize(Vbo* vbo);
  public:
    VboFinalizer() = default;
    ~VboFinalizer() override = default;

    bool VisitVbo(Vbo* vbo) override {
      PRT_ASSERT(vbo);
      Finalize(vbo);
      return true;
    }
  };
}

#endif //PRT_VBO_FINALIZER_H