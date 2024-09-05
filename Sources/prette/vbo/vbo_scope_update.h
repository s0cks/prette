#ifndef PRT_VBO_SCOPE_H
#error "Please #include <prette/vbo/vbo_scope.h> instead."
#endif //PRT_VBO_SCOPE_H

#ifndef PRT_VBO_SCOPE_UPDATE_H
#define PRT_VBO_SCOPE_UPDATE_H

#include "prette/vbo/vbo_scope.h"

namespace prt::vbo {
  class VboUpdateScopeBase : public VboBindScope {
  protected:
    explicit VboUpdateScopeBase(Vbo* vbo):
      VboBindScope(vbo) {
    }

    uword GetElementSize() const;
    void Update(const uword offset, const Region& source);
  public:
    ~VboUpdateScopeBase() override = default;
  };

  template<typename V>
  class VboUpdateScope : public VboUpdateScopeBase {
  public:
    typedef std::vector<V> VertexList;
  public:
    explicit VboUpdateScope(Vbo* vbo):
      VboUpdateScopeBase(vbo) {
    }
    ~VboUpdateScope() override = default;

    void Update(const uword offset, const V* data, const uword length) {
      const auto src = Region((uword) data, length * GetElementSize());
      VboUpdateScopeBase::Update(offset, src);
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