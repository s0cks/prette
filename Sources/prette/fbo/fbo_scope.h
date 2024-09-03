#ifndef PRT_FBO_SCOPE_H
#define PRT_FBO_SCOPE_H

#include "prette/fbo/fbo_id.h"

namespace prt::fbo {
  class Fbo;
  class FboScope {
  protected:
    Fbo* fbo_;
  public:
    explicit FboScope(Fbo* fbo):
      fbo_(fbo) {
    }
    virtual ~FboScope() = default;

    Fbo* GetFbo() const {
      return fbo_;
    }

    FboId GetFboId() const;
  };
}

#include "prette/fbo/fbo_scope_bind.h"

#endif //PRT_FBO_SCOPE_H