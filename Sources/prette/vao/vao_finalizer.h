#ifndef PRT_VAO_FINALIZER_H
#define PRT_VAO_FINALIZER_H

#include "prette/wsq.h"
#include "prette/counter.h"
#include "prette/platform.h"

namespace prt::vao {
  class Vao;
  class VaoFinalizer {
  protected:
    Counter<uint32_t> num_finalized_;
    WorkStealingQueue<uword> queue_;

    VaoFinalizer() = default;
    
    virtual void Submit(Vao* vao) {
      queue_.Push(reinterpret_cast<uword>(vao));
    }

    void Finalize(Vao* vao);
    void FinalizeQueue();
  public:
    ~VaoFinalizer() = default;
  };
}

#endif //PRT_VAO_FINALIZER_H