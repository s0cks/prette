#ifndef PRT_FONT_H
#define PRT_FONT_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <cstdio>
#include <cstdlib>
#include <glog/logging.h>

#include "prette/font/glyph.h"
#include "prette/font/glyph_map.h"
#include "prette/font/font_mesh.h"
#include "prette/program/program.h"

namespace prt::font {
  typedef FT_Face FontFace;

  class Font {
    friend class FontRenderer;

    static constexpr const FontSize kDefaultFontSize = 48;
  protected:
    Program* shader_;
    GlyphMap chars_;
    FontMesh mesh_;
  public:
    Font(const std::string& filename, const FontSize size = kDefaultFontSize);
    virtual ~Font() = default;

    std::string ToString() const;
  };

  void Initialize();
  void LoadTrueTypeFont(const std::string& name, FontFace* result);
  void SetFont(FontFace* font);
  FontFace* GetFont();
  FontFace* GetArialFont();

  bool LoadTrueTypeFontFrom(const std::string& filename, Font* result);
}

#endif //PRT_FONT_H