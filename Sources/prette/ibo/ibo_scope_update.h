#ifndef PRT_IBO_SCOPE_H
#error "Please #include <prette/ibo/ibo_scope.h> instead."
#endif //PRT_IBO_SCOPE_H

#ifndef PRT_IBO_SCOPE_UPDATE_H
#define PRT_IBO_SCOPE_UPDATE_H

#include "prette/ibo/ibo_scope.h"

namespace prt::ibo {
  template<typename I>
  class IboUpdateScope : public IboBindScope {
  public:
    explicit IboUpdateScope(Ibo* ibo):
      IboBindScope(ibo) {
      PRT_ASSERT(sizeof(I) == ibo->GetElementSize());
    }
    ~IboUpdateScope() override = default;

    void Update(const uword offset, const I* data, const uword length) {
      const auto total_size = length * GetIbo()->GetElementSize();
      const auto source = Region((uword) data, total_size);
      if(GetIbo()->IsEmpty()) {
        PRT_ASSERT(offset == 0);
        Ibo::InitBufferData(source, GetIbo()->GetUsage());
      } else {
        Ibo::UpdateBufferData(offset, source);
      }
      GetIbo()->SetLength(length);
    }

    inline void Update(const uword offset, const std::vector<I>& data) {
      return Update(offset, &data[0], data.size());
    }

    inline void Update(const I* data, const uword length) {
      return Update(0, data, length);
    }

    inline void Update(const std::vector<I>& data) {
      return Update(&data[0], data.size());
    }
  };
}

#endif //PRT_IBO_SCOPE_UPDATE_H