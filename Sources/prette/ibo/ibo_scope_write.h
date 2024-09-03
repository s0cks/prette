#ifndef PRT_IBO_SCOPE_H
#error "Please #include <prette/ibo/ibo_scope.h> instead."
#endif //PRT_IBO_SCOPE_H

#ifndef PRT_IBO_SCOPE_WRITE_H
#define PRT_IBO_SCOPE_WRITE_H

#include "prette/ibo/ibo_scope.h"

namespace prt::ibo {
  template<typename I>
  class IboWriteScope : public IboBindScope,
                        public WriteOnlyIboScope {
  public:
    explicit IboWriteScope(Ibo* ibo):
      IboBindScope(ibo),
      WriteOnlyIboScope(ibo) {
      PRT_ASSERT(sizeof(I) == ibo->GetElementSize());
    }
    ~IboWriteScope() override = default;
    //TODO: implement
  };
}

#endif //PRT_IBO_SCOPE_WRITE_H