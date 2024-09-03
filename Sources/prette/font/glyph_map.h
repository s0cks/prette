#ifndef PRT_GLYPH_MAP_H
#define PRT_GLYPH_MAP_H

#include <map>
#include "prette/font/glyph.h"

namespace prt::font {
  typedef std::map<GLchar, Glyph> GlyphMap;

  class GlyphMapGenerator {
  protected:
    FT_Face& face_;
    FT_UInt height_;

    GlyphMapGenerator(FT_Face& face, FT_UInt height):
      face_(face),
      height_(height) {
    }

    void GenerateCharacterMap(GlyphMap& chars) {
      glEnable(GL_CULL_FACE);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      FT_Set_Pixel_Sizes(face_, 0, height_);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      for(unsigned char c = 0; c < 128; c++) {
        if(FT_Load_Char(face_, c, FT_LOAD_RENDER)) {
          LOG(ERROR) << "failed to load glyph for '" << c << "'";
          continue;
        }

        auto& glyph = face_->glyph;
        chars.insert(std::pair<char, Glyph>(c, Glyph(glyph)));
      }
      glBindTexture(GL_TEXTURE_2D, 0);
      glDisable(GL_BLEND);
      glDisable(GL_CULL_FACE);
    }
  public:
    virtual ~GlyphMapGenerator() = default;
    DEFINE_NON_COPYABLE_TYPE(GlyphMapGenerator);
  public:
    static void Generate(FT_Face& face, GlyphMap& chars, const FT_UInt height = 48);
    static void Generate(const std::string& name, GlyphMap& chars, const FT_UInt height = 48);
  };
}

#endif //PRT_GLYPH_MAP_H