#include "prette/font/glyph_map.h"
#include "prette/font/font.h"

namespace prt::font {
  void GlyphMapGenerator::Generate(FT_Face& face, GlyphMap& chars, const FT_UInt height) {
    GlyphMapGenerator gen(face, height);
    return gen.GenerateCharacterMap(chars);
  }

  void GlyphMapGenerator::Generate(const std::string& name, GlyphMap& chars, const FT_UInt height) {
    FontFace face;
    LoadTrueTypeFont(name, &face);
    return GlyphMapGenerator::Generate(face, chars, height);
  }
}