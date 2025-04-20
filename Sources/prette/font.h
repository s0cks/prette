#ifndef PRT_FONT_H
#define PRT_FONT_H

#include <freetype/freetype.h>
#include <harfbuzz/hb.h>

#include <string>

#include "prette/common.h"
#include "prette/glm.h"

namespace prt {
struct ShapingPair {
  uint32_t glyph_index;
  glm::u32vec2 advance;
  glm::u32vec2 offset;
};

class ShapingIterator {
 private:
  hb_glyph_info_t* glyph_infos_;
  hb_glyph_position_t* glyph_positions_;
  unsigned index_;

 public:
  ShapingIterator(hb_glyph_info_t* glyph_infos, hb_glyph_position_t* glyph_positions, const unsigned index) :
    glyph_infos_(glyph_infos),
    glyph_positions_(glyph_positions),
    index_(index) {}
  ~ShapingIterator() = default;

  auto operator++() -> ShapingIterator& {
    index_ += 1;
    return *this;
  }

  auto operator==(const ShapingIterator& rhs) {
    return index_ == rhs.index_;
  }

  auto operator*() const -> ShapingPair;
};

class ShapingView {
  DEFINE_NON_COPYABLE_TYPE(ShapingView);

 private:
  hb_buffer_t* buffer_;
  hb_glyph_info_t* glyph_info_;
  hb_glyph_position_t* glyph_pos_;
  unsigned glyph_count_;

 public:
  ShapingView(hb_buffer_t* buffer, hb_glyph_info_t* glyph_infos, hb_glyph_position_t* glyph_positiosns,
              const unsigned glyh_count);
  ~ShapingView();
};

class Font {
 private:
  FT_Library library_;
  hb_font_t* font_;

 public:
  Font();
  ~Font();

  auto Shape(std::string text) const -> ShapingView;

 public:
  static auto Load(std::string name, const long size) -> Font*;
};
}  // namespace prt

#endif  // PRT_FONT_H
