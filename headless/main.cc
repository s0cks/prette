#include <backward.hpp>
#include <chrono>
#include <cstdlib>
#include <glog/logging.h>
#include <string>
#include <units.h>

#include "prette/asset/asset.h"
#include "prette/asset/asset_registry.h"
#include "prette/common.h"
#include "prette/engine/engine.h"
#include "prette/engine/engine_event.h"
#include "prette/flags.h"
#include "prette/uri.h"

using namespace prt;

auto main(int argc, char** argv) -> int {
  Engine::Init(argc, argv);

  asset::AssetIndexer* indexers = new asset::ShaderAssetIndexer();
  asset::AssetRegistry::InitRegistry(FLAGS_resources, indexers);
  LOG(INFO) << "assets:";
  LOG_IF(FATAL, !asset::AssetRegistry::GetRegistry()->VistAllKeysAndAssets([](const std::string& k,
                                                                              const asset::Asset& asset) {
    LOG(INFO) << " - " << k << " => " << asset;
    return true;
  })) << "failed to print available assets.";

  DLOG(INFO) << "looking for grid vertex shader source asset....";
  PRT_PROFILING_BEGIN(get_asset)
  const auto grid_vert_shader_source = asset::AssetRegistry::GetRegistry()->GetAsset(uri("shader:grid.vert"));
  PRT_PROFILING_END(get_asset)
  LOG_IF(FATAL, !grid_vert_shader_source) << "failed to find grid vertex shader source";
  LOG(INFO) << "grid vertex shader source: " << (*grid_vert_shader_source) << "  ("
            << std::chrono::duration_cast<std::chrono::milliseconds>(get_asset_duration) << ").";

  // if (VLOG_IS_ON(1)) {
  //   SUBSCRIBE_AND_LOG(GetConfigEventObservable(), INFO);
  //   SUBSCRIBE_AND_LOG(GetSettingsEventObservable(), INFO);
  //   SUBSCRIBE_AND_LOG(GetLuaStateEventObservable(), INFO);
  //   SUBSCRIBE_AND_LOG(GetWorldEventObservable(), INFO);
  //   SUBSCRIBE_AND_LOG(GetNonTickEngineEventObservable(), INFO);
  // }
  // const auto engine = GetEngine();
  // ASSERT(engine);
  // return engine->Run();
  return EXIT_SUCCESS;
}