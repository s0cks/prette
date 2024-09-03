#ifndef PRT_UBO_SCOPE_H
#define PRT_UBO_SCOPE_H

#include "prette/ubo/ubo.h"
#include "prette/gfx_mapped_buffer_scope.h"

namespace prt::ubo {
  class UboScope {
  protected:
    Ubo* ubo_;

    explicit UboScope(Ubo* ubo):
      ubo_(ubo) {
    }
  public:
    virtual ~UboScope() = default;

    Ubo* GetUbo() const {
      return ubo_;
    }

    UboId GetUboId() const {
      return GetUbo()->GetId();
    }
  };

  class ReadOnlyUboScope : public gfx::ReadOnlyMappedBufferScope<GL_UNIFORM_BUFFER> { //TODO: use Ubo::kGlTarget
  public:
    explicit ReadOnlyUboScope(Ubo* ubo);
    ~ReadOnlyUboScope() override = default;
  };

  class WriteOnlyUboScope : public gfx::WriteOnlyMappedBufferScope<GL_UNIFORM_BUFFER> { //TODO: use Ubo::kGlTarget
  public:
    explicit WriteOnlyUboScope(Ubo* ubo);
    ~WriteOnlyUboScope() override = default;
  };
}

#include "prette/ubo/ubo_scope_bind.h"
#include "prette/ubo/ubo_scope_read.h"
#include "prette/ubo/ubo_scope_update.h"

#endif //PRT_UBO_SCOPE_H