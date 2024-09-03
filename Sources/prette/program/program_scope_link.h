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
  protected:
    ShaderIdList linked_;
  public:
    explicit ProgramLinkScope(Program* program);
    ~ProgramLinkScope();

    void Attach(const ShaderId id);
    void AttachAll(const ShaderId* ids, const uword num_ids);

    inline void AttachAll(const ShaderIdList& ids) {
      return AttachAll(&ids[0], ids.size());
    }

    ProgramLinkStatus Link() const;

    ShaderIdList::const_iterator begin() const {
      return std::begin(linked_);
    }

    ShaderIdList::const_iterator end() const {
      return std::end(linked_);
    }

    ProgramLinkScope& operator<<(const ShaderId id) {
      Attach(id);
      return *this;
    }

    ProgramLinkScope& operator<<(const ShaderIdList& shaders) {
      if(!shaders.empty())
        AttachAll(shaders);
      return *this;
    }
  };
}

#endif //PRT_PROGRAM_SCOPE_LINK_H