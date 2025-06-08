#include "prette/renderer_event.h"

#include <string>

#include "prette/renderer_state.h"
#include "prette/to_string.h"

namespace prt {
// renderer events
#define DEFINE_TOSTRING(Name)                       \
  auto Name##Event::ToString() const->std::string { \
    ToStringHelper<Name##Event> helper{};           \
    return helper;                                  \
  }
FOR_EACH_RENDERER_STATE(DEFINE_TOSTRING);
DEFINE_TOSTRING(UpdateBuffers);
DEFINE_TOSTRING(InitRenderPasses);
DEFINE_TOSTRING(InitDescriptorSets);
DEFINE_TOSTRING(InitPipelineLayouts);
DEFINE_TOSTRING(InitGraphicsPipelines);
DEFINE_TOSTRING(InitBuffers);
DEFINE_TOSTRING(InitTextures);
DEFINE_TOSTRING(InitMeshes);
DEFINE_TOSTRING(InitMaterials);
#undef DEFINE_TOSTRING

// frame events
#define DEFINE_TOSTRING(Name)                            \
  auto Name##FrameEvent::ToString() const->std::string { \
    ToStringHelper<Name##FrameEvent> helper{};           \
    return helper;                                       \
  }
DEFINE_TOSTRING(Pre);
DEFINE_TOSTRING(Post);
#undef DEFINE_TOSTRING
}  // namespace prt