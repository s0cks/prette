#include "prette/engine/engine_state_terminated.h"

#include "prette/engine/engine.h"
#include "prette/engine/engine_event.h"
#include "prette/engine/engine_state.h"

namespace prt {
#define __ engine->

ENGINE_STATE_ENTER_F(Terminated) {
  // TODO:
  //  Driver::Get()->WaitDeviceIdle();
  //  GetCameraManager()->FinalizeAllCameras();
  //  vk::RenderPassFinalizer::FinalizeAll();
  //  vk::RenderPipelineFinalizer::FinalizeAll();
  //  vk::GetPipelineLayoutManager()->FinalizeAll();
  //  vk::PipelineCacheFinalizer::FinalizeAll();
  //  vk::DescriptorSetFinalizer::FinalizeAll();
  // ShutdownCallback::ApplyAll(engine->GetShutdownCallbacks());
  Engine::PublishTerminatedEvent();
}

ENGINE_STATE_EXIT_F(Terminated) {
  // TODO: DriverFinalizer::Finalize();
}

#undef __
}  // namespace prt