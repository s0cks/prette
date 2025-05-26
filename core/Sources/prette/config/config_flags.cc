#include "prette/config/config_flags.h"

#include <gflags/gflags.h>

namespace prt {
DEFINE_string(config, kDefaultConfigFilename, "The name of the config file to load.");
}