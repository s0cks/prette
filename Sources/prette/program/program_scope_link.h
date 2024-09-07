#ifndef PRT_PROGRAM_SCOPE_H
#error "Please #include <prette/program/program_scope.h> instead."
#endif //PRT_PROGRAM_SCOPE_H

#ifndef PRT_PROGRAM_SCOPE_LINK_H
#define PRT_PROGRAM_SCOPE_LINK_H

#include "prette/shader/shader_id.h"
#include "prette/program/program_id.h"
#include "prette/program/program_scope.h"
#include "prette/program/program_status.h"

namespace prt::program {
  class ProgramLinkScope : public ProgramScope {
    DEFINE_NON_COPYABLE_TYPE(ProgramLinkScope);
  private:
    ShaderIdList linked_;
  public:
    explicit ProgramLinkScope(Program* program);
    ~ProgramLinkScope() override;

    void Attach(const ShaderId id);
    void AttachAll(const ShaderId* ids, const uword num_ids);

    inline void AttachAll(const ShaderIdList& ids) {
      return AttachAll(&ids[0], ids.size());
    }

    auto Link() const -> ProgramLinkStatus;

    auto begin() const -> ShaderIdList::const_iterator {
      return std::begin(linked_);
    }

    auto end() const -> ShaderIdList::const_iterator {
      return std::end(linked_);
    }

    auto operator<<(const ShaderId id) -> ProgramLinkScope& {
      Attach(id);
      return *this;
    }

    auto operator<<(const ShaderIdList& shaders) -> ProgramLinkScope& {
      if(!shaders.empty())
        AttachAll(shaders);
      return *this;
    }
  };
}

#endif //PRT_PROGRAM_SCOPE_LINK_H