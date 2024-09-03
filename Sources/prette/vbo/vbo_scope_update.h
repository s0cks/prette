#ifndef PRT_VBO_SCOPE_H
#error "Please #include <prette/vbo/vbo_scope.h> instead."
#endif //PRT_VBO_SCOPE_H

#ifndef PRT_VBO_SCOPE_UPDATE_H
#define PRT_VBO_SCOPE_UPDATE_H

#include "prette/vbo/vbo_scope.h"

namespace prt::vbo {
  template<typename V>
  class VboUpdateScope : public VboBindScope {
  public:
    typedef std::vector<V> VertexList;
  public:
    explicit VboUpdateScope(Vbo* vbo):
      VboBindScope(vbo) {
    }
    ~VboUpdateScope() override = default;

    void Update(const uword offset, const V* data, const uword length) {
      PRT_ASSERT(offset >= 0);
      PRT_ASSERT(data);
      PRT_ASSERT(length > 0);
      const auto total_size = GetVbo()->GetElementSize() * length;
      const auto src = Region((uword) data, total_size);
      if(GetVbo()->IsEmpty()) {
        PRT_ASSERT(offset == 0);
        Vbo::InitData(src, GetVbo()->GetUsage());
      } else {
        Vbo::UpdateVboData(offset, src);
      }
      GetVbo()->SetLength(length);
    }

    void Update(const V* data, const uword length) {
      PRT_ASSERT(data);
      PRT_ASSERT(length > 0);
      return Update(0, data, length);
    }

    inline void Update(const VertexList& data) {
      PRT_ASSERT(!data.empty());
      return Update(&data[0], data.size()); 
    }
  };
}

#endif //PRT_VBO_SCOPE_UPDATE_H