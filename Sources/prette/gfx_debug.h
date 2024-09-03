#ifndef PRT_GFX_DEBUG_H
#define PRT_GFX_DEBUG_H

#include "prette/flags.h"

namespace prt {
  namespace gfx {
#ifndef PRT_DEBUG
DECLARE_bool(enable_gfx_debug);
#endif//PRT_DEBUG

    void EnableDebug(); //TODO: cleanup this function
    bool IsDebugEnabled();
  }
}

#endif //PRT_GFX_DEBUG_H