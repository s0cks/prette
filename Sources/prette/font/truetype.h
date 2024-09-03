#ifndef PRT_FONT_TRUETYPE_H
#define PRT_FONT_TRUETYPE_H

#include "prette/gfx.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace prt::font {
  typedef FT_UInt FontSize;
}

#endif //PRT_FONT_TRUETYPE_H