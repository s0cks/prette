#include <imgui.h>
#include <stb_include.h>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/chunk/chunk_metadata.h"
#include "prette/config/config.h"  // IWYU pragma: keep
#include "prette/glm.h"
#include "prette/keyboard/keyboard.h"
#include "prette/lua.h"
#include "prette/mouse/mouse.h"
#include "prette/mouse/mouse_event.h"
#include "prette/renderer.h"
#include "prette/renderer_event.h"
#include "prette/rx.h"
#include "prette/settings/settings.h"
#include "prette/tile.h"
#include "prette/tile_mesh.h"
#include "prette/world/world.h"
#include "prette/world/world_manager.h"

// clang-format on
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
// clang-format on

#ifdef PRT_GLFW
#include <GLFW/glfw3.h>
#endif  // PRT_GLFW

#include <backward.hpp>
#include <cstdlib>
#include <glog/logging.h>
#include <units.h>

#include "prette/common.h"
#include "prette/engine/engine.h"
#include "prette/engine/engine_event.h"
#include "prette/gfx.h"
#include "prette/window/window.h"

using namespace prt;

auto main(int argc, char** argv) -> int {
  Engine::Init(argc, argv);
  Renderer::Init();

#ifdef PRT_DEBUG
  if (VLOG_IS_ON(1)) {
    SUBSCRIBE_AND_LOG(GetConfigEventObservable(), INFO);
    SUBSCRIBE_AND_LOG(GetSettingsEventObservable(), INFO);
    SUBSCRIBE_AND_LOG(GetWorldEventObservable(), INFO);
    SUBSCRIBE_AND_LOG(GetNonTickEngineEventObservable(), INFO);
    SUBSCRIBE_AND_LOG(GetNonFrameRendererEventObservable(), INFO);
#ifdef PRT_ENABLE_LUA
    SUBSCRIBE_AND_LOG(GetLuaStateEventObservable(), INFO);
#endif  // PRT_ENABLE_LUA
  }
#endif  // PRT_DEBUG

  OnKeyPressed(GLFW_KEY_ESCAPE).subscribe([](KeyStateEvent* event) {
    ASSERT(event);
    const auto engine = GetEngine();
    ASSERT(engine);
    engine->Shutdown();
  });

  DLOG(INFO) << "tile normal: " << glm::to_string(GetTileNormal());

  OnMouseMotion([](MouseMotionEvent* event) {
    const auto pos = Mouse::Get()->GetWorldPos();
    ChunkPos chunk_pos(static_cast<uint32_t>(pos.x) / kChunkWidth, static_cast<uint32_t>(pos.y) / kChunkHeight);
    const auto chunk = GetWorld()->GetChunkAt(chunk_pos);
    if (!chunk)
      return;
    ASSERT(chunk);
    chunk->VisitTiles([&pos](Tile* tile) {
      if (!tile->Contains(pos)) {
        if (tile->IsHovering())
          tile->SetHovering(false);
        return true;  // skip
      }
      tile->SetHovering(true);
      return true;
    });
  });
  const auto engine = GetEngine();
  ASSERT(engine);
  return engine->Run();
}

// OnInitDescriptorSets([](InitDescriptorSetsEvent* event) {
//   const auto descriptors = vk::DescriptorSet::FromJsonFile(fs::path(FLAGS_resources) / "descriptor-set-test.json");
//   ASSERT_INITIALIZED(descriptors);
// });

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