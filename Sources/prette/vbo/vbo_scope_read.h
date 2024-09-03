#ifndef PRT_VBO_SCOPE_H
#error "Please #include <prette/vbo/vbo_scope.h> instead."
#endif //PRT_VBO_SCOPE_H

#ifndef PRT_VBO_SCOPE_READ_H
#define PRT_VBO_SCOPE_READ_H

#include "prette/vbo/vbo_scope.h"
#include "prette/gfx_mapped_buffer_scope.h"

namespace prt::vbo {
  template<class V>
  class VboReadScope : public VboBindScope,
                       public ReadOnlyVboScope {
  public:
    explicit VboReadScope(Vbo* vbo):
      VboBindScope(vbo),
      ReadOnlyVboScope(vbo) {
    }
    ~VboReadScope() override = default;

    rx::observable<V> ReadAll() const {
      if(!IsMapped())
        return rx::observable<>::empty<V>();
        
      return rx::observable<>::create<V>([this](rx::subscriber<V> s) {
        Iterator<V> iter(this);
        while(iter.HasNext())
          s.on_next(*iter.Next());
        s.on_completed();
      });
    }

#ifdef PRT_DEBUG
    template<const google::LogSeverity Severity = google::INFO>
    void PrintAll() const {
      LOG_AT_LEVEL(Severity) << GetVbo()->ToString() << " Vertices:";
      return ReadAll()
        .as_blocking()
        .subscribe([](const V& vertex) {
          LOG_AT_LEVEL(Severity) << " - " << vertex;
        });
    }
#endif //PRT_DEBUG
  };
}

#endif //PRT_VBO_SCOPE_READ_H