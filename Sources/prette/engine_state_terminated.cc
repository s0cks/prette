#include "prette/camera_manager.h"
#include "prette/descriptor_set.h"
#include "prette/engine.h"
#include "prette/engine_event.h"
#include "prette/engine_state.h"
#include "prette/gfx.h"
#include "prette/gfx_driver.h"
#include "prette/gfx_vk.h"
#include "prette/pipeline.h"
#include "prette/pipeline_cache.h"
#include "prette/pipeline_layout.h"
#include "prette/render_pass.h"

namespace prt {
#define __ engine->

ENGINE_STATE_ENTER_F(Terminated) {
  Driver::Get()->WaitDeviceIdle();
  GetCameraManager()->FinalizeAllCameras();
  vk::RenderPassFinalizer::FinalizeAll();
  vk::RenderPipelineFinalizer::FinalizeAll();
  vk::GetPipelineLayoutManager()->FinalizeAll();
  vk::PipelineCacheFinalizer::FinalizeAll();
  vk::DescriptorSetFinalizer::FinalizeAll();
  ShutdownCallback::ApplyAll(engine->GetShutdownCallbacks());
  Engine::PublishTerminatedEvent();
}

ENGINE_STATE_EXIT_F(Terminated) {
  DriverFinalizer::Finalize();
}

#undef __
}  // namespace prt