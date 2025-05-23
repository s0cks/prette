#include <stb_include.h>
#include <vulkan/vulkan_core.h>

#include "prette/camera_manager.h"
#include "prette/config_manager.h"
#include "prette/engine_event.h"
#include "prette/flags.h"
#include "prette/gfx_driver.h"
#include "prette/gfx_driver_event.h"
#include "prette/gfx_vk.h"
#include "prette/gui_renderer.h"
#include "prette/jsonnet.h"
#include "prette/mouse.h"
#include "prette/rx.h"
#include "prette/tile.h"
#include "prette/world_manager.h"

// clang-format on
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
// clang-format on

#ifdef PRT_GLFW
#include <GLFW/glfw3.h>
#endif  // PRT_GLFW

#include <backward.hpp>
#include <cstdlib>
#include <exception>
#include <glog/logging.h>
#include <units.h>

#include "prette/camera.h"
#include "prette/chunk.h"
#include "prette/common.h"
#include "prette/config.h"
#include "prette/crash_report.h"
#include "prette/ecl.h"
#include "prette/engine.h"
#include "prette/gfx.h"
#include "prette/gui.h"
#include "prette/keyboard.h"
#include "prette/mouse_event.h"
#include "prette/os_thread.h"
#include "prette/platform.h"
#include "prette/renderer.h"
#include "prette/scene_renderer.h"
#include "prette/settings.h"
#include "prette/signals.h"
#include "prette/swapchain.h"
#include "prette/swapchain_event.h"
#include "prette/window.h"
#include "prette/window_event.h"
#include "prette/world.h"
#include "prette/world_manager.h"

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

  ConfigManager::Init();
  // Settings::Init();
  LuaState::Init();
  // gfx::Init();
  // Window::Init();
  // InitCameraManager();
  Engine::Init();
  // WorldManager::Init();
  // Driver::Init();
  // gui::Init();
  // Renderer::Init();
  // InitSceneRenderer();
  // InitGuiRenderer();

  InitLisp(argc, argv);
  InitJsonnet();

  // const auto path = fs::path(FLAGS_resources) / "test.txt";
  // OnPostInitEvent().subscribe([path](PostInitEvent* event) {
  //   DLOG(INFO) << "opening: " << path << "....";
  //   async::OpenFileReadOnly(path, [path](async::FileHandle* file) {
  //     DLOG(INFO) << path << " opened!";
  //     async::ReadFile(file, [path, file](const std::vector<uint8_t>& data) {
  //       DLOG(INFO) << path << ": " << ((const char*)data.data());  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
  //       async::CloseFile(file, [path](int status) {
  //         DLOG(INFO) << path << " closed!: " << status;
  //       });
  //     });
  //   });
  // });

  // OnMouseMotionEvent().subscribe([](MouseMotionEvent* event) {
  //   const auto pos = Mouse::Get()->GetWorldPos();
  //   ChunkPos chunk_pos(static_cast<uint32_t>(pos.x) / kChunkWidth, static_cast<uint32_t>(pos.y) / kChunkHeight);
  //   const auto chunk = GetWorld()->GetChunkAt(chunk_pos);
  //   if (!chunk)
  //     return;
  //   ASSERT(chunk);
  //   chunk->VisitTiles([&pos](Tile* tile) {
  //     if (!tile->Contains(pos)) {
  //       if (tile->IsHovering())
  //         tile->SetHovering(false);
  //       return true;  // skip
  //     }
  //     tile->SetHovering(true);
  //     return true;
  //   });
  // });
  // OnMousePressed(GLFW_MOUSE_BUTTON_1).subscribe([](MouseButtonStateEvent* event) {
  //   const auto pos = Mouse::Get()->GetWorldPos();
  //   ChunkPos chunk_pos(static_cast<uint32_t>(pos.x) / kChunkWidth, static_cast<uint32_t>(pos.y) / kChunkHeight);
  //   const auto chunk = World::Get()->GetChunkAt(chunk_pos);
  //   if (!chunk)
  //     return;
  //   ASSERT(chunk);
  //   chunk->VisitTiles([&pos](Tile* tile) {
  //     if (tile->Contains(pos)) {
  //       tile->SetMaterial(tile->GetMaterial() == 2 ? kDefaultTileMaterial : 2);
  //     }
  //     return true;
  //   });
  // });

  // html::Init();
  // const auto doc = html::FromString(
  //     "<html><body><p style=\"display: block; background-color: white;\">Hello World</p></body></html>");
  // const auto max_width = 1080;
  // const auto best_width = doc->render(max_width);
  // if (best_width < max_width) {
  //   doc->render(best_width);
  // }
  const auto engine = GetEngine();
  ASSERT(engine);
  return engine->Run();
}