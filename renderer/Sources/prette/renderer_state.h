#ifndef PRT_RENDERER_STATE_H
#define PRT_RENDERER_STATE_H

#include <ostream>
#include <typeinfo>

namespace prt {
#define FOR_EACH_RENDERER_STATE(V) \
  V(RendererCreated)               \
  V(RendererDestroyed)

enum class RendererState {
#define DEFINE_STATE(Name) k##Name,
  FOR_EACH_RENDERER_STATE(DEFINE_STATE)
#undef DEFINE_STATE
};

static inline auto operator<<(std::ostream& stream, const RendererState& rhs) -> std::ostream& {
  switch (rhs) {
#define DEFINE_TO_STRING(Name) \
  case RendererState::k##Name: \
    return stream << #Name;
    FOR_EACH_RENDERER_STATE(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
    default:
      return stream << "undefined " << typeid(RendererState).name();
  }
}
}  // namespace prt

#endif  // PRT_RENDERER_STATE_H
