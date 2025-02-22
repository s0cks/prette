#include <stb_include.h>

// clang-format on
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
// clang-format on

#include <GLFW/glfw3.h>
#include <glog/logging.h>
#include <units.h>
#include <uv.h>

#include <backward.hpp>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <termcolor/termcolor.hpp>

#include "prette/camera.h"
#include "prette/config.h"
#include "prette/engine.h"
#include "prette/glm.h"
#include "prette/gui.h"
#include "prette/keyboard.h"
#include "prette/os_thread.h"
#include "prette/registry.h"
#include "prette/renderer.h"
#include "prette/scene_renderer.h"
#include "prette/signals.h"
#include "prette/swapchain.h"
#include "prette/test_generated.h"
#include "prette/window.h"

using namespace prt;

static inline void OnUnhandledException() {
  CrashReport report(CrashReportCause::New(std::current_exception()));
  report.Print();
  LOG(FATAL) << "unhandled exception occured.";
}

auto main(int argc, char** argv) -> int {
  // ::google::InstallPrefixFormatter(&MyPrefixFormatter);
  ::google::InitGoogleLogging(argv[0]);
  ::google::ParseCommandLineFlags(&argc, &argv, true);
  srand(time(nullptr));
  InitSignalHandlers();
  std::set_terminate(OnUnhandledException);
  LOG_IF(FATAL, !SetCurrentThreadName("main")) << "failed to set main thread name.";
  Config::Load();
  gfx::Init();
  LuaState::Init();
  Window::Init();
  Camera::Init();
  Engine::Init();
  Driver::Init();
  gui::Init();
  Renderer::Init();
  SwapChain::Init();
  SceneRenderer::Init();
  GuiRenderer::Init();
  const auto engine = Engine::Get();
  ASSERT(engine);
  return engine->Run();
}