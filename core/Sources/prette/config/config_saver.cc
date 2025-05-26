#include "prette/config/config_saver.h"

#include <chrono>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/config/config.h"
#include "prette/config/config_event.h"
#include "prette/config/config_flags.h"

namespace prt {
void ConfigSaver::SaveConfig(Config* config) {
  ASSERT(config);
  DVLOG(1) << "saving config....";
  PRT_PROFILING_BEGIN(save);
  Config::Publish<ConfigSavingEvent>();
  const auto status = config->Save();
  if (!status) {
    LOG(ERROR) << "failed to save config to " << GetConfigPath() << ": " << status;
    return;
  }
  Config::Publish<ConfigSavedEvent>();
  PRT_PROFILING_END(save);
  DLOG_IF(INFO, VLOG_IS_ON(1)) << "config saved in "
                               << std::chrono::duration_cast<std::chrono::milliseconds>(save_duration).count() << "ms.";
}
}  // namespace prt