#ifndef PRT_IBO_SCOPE_H
#define PRT_IBO_SCOPE_H

#include "prette/ibo/ibo.h"
#include "prette/gfx_mapped_buffer_scope.h"

namespace prt::ibo {
  class IboScope {
  protected:
    Ibo* ibo_;

    explicit IboScope(Ibo* ibo):
      ibo_(ibo) {
    }
  public:
    virtual ~IboScope() = default;
    
    Ibo* GetIbo() const {
      return ibo_;
    }

    IboId GetIboId() const;
  };

  class ReadOnlyIboScope : public gfx::ReadOnlyMappedBufferScope<kGlTarget> {
  public:
    explicit ReadOnlyIboScope(Ibo* ibo);
    ~ReadOnlyIboScope() override = default;
  };

  class WriteOnlyIboScope : public gfx::WriteOnlyMappedBufferScope<kGlTarget> {
  public:
    explicit WriteOnlyIboScope(Ibo* ibo);
    ~WriteOnlyIboScope() override = default;
  };
}

#include "prette/ibo/ibo_scope_bind.h"
#include "prette/ibo/ibo_scope_draw.h"
#include "prette/ibo/ibo_scope_read.h"
#include "prette/ibo/ibo_scope_write.h"
#include "prette/ibo/ibo_scope_update.h"

#endif //PRT_IBO_SCOPE_H