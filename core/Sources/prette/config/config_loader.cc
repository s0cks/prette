#include "prette/config/config_loader.h"

#include <chrono>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/config/config.h"
#include "prette/config/config_flags.h"

namespace prt {
void ConfigLoader::LoadConfig(Config* config) {
  ASSERT(config);
  DVLOG(1) << "loading config....";
  PRT_PROFILING_BEGIN(load);
  Config::PublishConfigLoadingEvent();
  const auto status = config->Load();
  if (!status) {
    LOG(ERROR) << "failed to load config from: " << GetConfigPath() << ": " << status;
    return;
  }
  Config::PublishConfigLoadedEvent();
  PRT_PROFILING_END(load);
  DLOG_IF(INFO, VLOG_IS_ON(1)) << "config loaded in "
                               << std::chrono::duration_cast<std::chrono::milliseconds>(load_duration).count() << "ms.";
}
}  // namespace prt