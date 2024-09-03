#ifndef PRT_IBO_SCOPE_H
#error "Please #include <prette/ibo/ibo_scope.h> instead."
#endif //PRT_IBO_SCOPE_H

#ifndef PRT_IBO_SCOPE_READ_H
#define PRT_IBO_SCOPE_READ_H

#include "prette/ibo/ibo_scope.h"

namespace prt::ibo {
  template<typename I>
  class IboReadScope : public IboBindScope,
                       public ReadOnlyIboScope {
  public:
    explicit IboReadScope(Ibo* ibo):
      IboBindScope(ibo),
      ReadOnlyIboScope(ibo) {
      PRT_ASSERT(sizeof(I) == ibo->GetElementSize());
    }
    ~IboReadScope() override = default;

    const I* begin() const {
      return (const I*) GetStartingAddressPointer();
    }

    const I* end() const {
      return (const I*) GetEndingAddressPointer();
    }

    rx::observable<I> ToObservable() const {
      if(!IsMapped())
        return rx::observable<>::empty<I>();
      return rx::observable<>::create<I>([this](rx::subscriber<I> s) {
        Iterator<I> iter(this);
        while(iter.HasNext())
          s.on_next(iter.Next());
        s.on_completed();
      });
    }
  };
}

#endif //PRT_IBO_SCOPE_READ_H