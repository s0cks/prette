#ifndef PRT_UBO_SCOPE_H
#error "Please #include <prette/ubo/ubo_scope.h> instead."
#endif //PRT_UBO_SCOPE_H

#ifndef PRT_UBO_SCOPE_READ_H
#define PRT_UBO_SCOPE_READ_H

#include "prette/ubo/ubo_scope.h"

namespace prt::ubo {
  template<typename T>
  class UboReadScope : public UboBindScope,
                       public ReadOnlyUboScope {
  public:
    explicit UboReadScope(Ubo* ubo):
      UboBindScope(ubo),
      ReadOnlyUboScope(ubo) {
    }
    ~UboReadScope() override = default;

    rx::observable<T> ReadAll() const {
      if(!IsMapped())
        return rx::observable<>::empty<T>();
        
      return rx::observable<>::create<T>([this](rx::subscriber<T> s) {
        Iterator<T> iter(this);
        while(iter.HasNext())
          s.on_next(iter.Next());
        s.on_completed();
      });
    }
  };
}

#endif //PRT_UBO_SCOPE_READ_H