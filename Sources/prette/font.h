#ifndef PRT_FONT_H
#define PRT_FONT_H

#include "prette/freetype.h"
#include "prette/rect.h"

namespace prt {
class TextureAtlas;
struct Glyph {
  using Face = FT_Face;
  using Index = FT_UInt;
  using GlyphTexture = std::shared_ptr<vk::Image>;
  static constexpr const auto kDefaultGlyphWidth = 64;
  static constexpr const auto kDefaultGlyphHeight = 64;
  static constexpr const auto kDefaultGlyphAdvance = 64;

  Face face;
  Index index;
  Rect bounds;
  int advance;
  GlyphTexture texture;

  Glyph(Face face, Index index, TextureAtlas* atlas);

 private:
  auto CreateTextureFromBitmap(TextureAtlas* atlas) -> GlyphTexture;
};

class Font;
}  // namespace prt

#endif  // PRT_FONT_H
