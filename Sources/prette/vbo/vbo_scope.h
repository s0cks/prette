#ifndef PRT_VBO_SCOPE_H
#define PRT_VBO_SCOPE_H

#include "prette/vbo/vbo.h"
#include "prette/gfx_mapped_buffer_scope.h"

namespace prt::vbo {
  class Vbo;
  class VboScope {
  protected:
    Vbo* vbo_;
    
    explicit VboScope(Vbo* vbo):
      vbo_(vbo) {
      PRT_ASSERT(vbo);
    }
  public:
    virtual ~VboScope() = default;

    Vbo* GetVbo() const {
      return vbo_;
    }

    VboId GetVboId() const;
  };

  class ReadOnlyVboScope : public gfx::ReadOnlyMappedBufferScope<Vbo::kGlTarget> { //TODO: use Vbo::kGlType
  public:
    explicit ReadOnlyVboScope(Vbo* vbo);
    ~ReadOnlyVboScope() override = default;
  };

  class WriteOnlyVboScope : public gfx::WriteOnlyMappedBufferScope<Vbo::kGlTarget> { //TODO: use Vbo::kGlType
  public:
    explicit WriteOnlyVboScope(Vbo* vbo);
    ~WriteOnlyVboScope() override = default;
  };
}

#include "prette/vbo/vbo_scope_bind.h"
#include "prette/vbo/vbo_scope_read.h"
#include "prette/vbo/vbo_scope_draw.h"
#include "prette/vbo/vbo_scope_update.h"

#endif //PRT_VBO_SCOPE_H