#include "prette/font.h"

#include <stdexcept>
#include <vector>

#include "prette/common.h"
#include "prette/freetype.h"
#include "prette/platform.h"
#include "prette/texture_atlas.h"
#include "prette/vk.h"

namespace prt {
Glyph::Glyph(Face face, Index index, TextureAtlas* atlas) {
  auto error = FT_Load_Glyph(face, index, FT_LOAD_DEFAULT);
  if (error)
    throw std::runtime_error("failed to load glyph");
  const auto& slot = face->glyph;
  error = FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);
  if (error)
    throw std::runtime_error("failed to render glyph");
  const auto& left = slot->bitmap_left;
  const auto& top = slot->bitmap_top;
  const auto width = static_cast<int>(slot->metrics.width / kDefaultGlyphWidth);
  const auto height = static_cast<int>(slot->metrics.height / kDefaultGlyphHeight);
  bounds = VkRect2D{
      .offset =
          {
              .x = left,
              .y = top,
          },
      .extent =
          {
              .width = width,
              .height = height,
          },
  };
  advance = static_cast<int>(slot->advance.x / kDefaultGlyphAdvance);
  texture = CreateTextureFromBitmap(atlas);
}

static inline auto IsValidBitmap(const FT_Bitmap& rhs) -> bool {
  return rhs.pixel_mode != FT_PIXEL_MODE_GRAY || rhs.num_grays != 256;
}

auto Glyph::CreateTextureFromBitmap(TextureAtlas* atlas) -> GlyphTexture {
  const auto& slot = face->glyph;
  if (IsValidBitmap(slot->bitmap))
    throw std::runtime_error("unsupported pixel mode");
  const auto buffer_size = CalcBufferSize(slot->bitmap);
  if (buffer_size == 0)
    return nullptr;
  std::vector<uint8_t> buffer(buffer_size);
  uint8_t* src = slot->bitmap.buffer;
  uint8_t* startOfLine = src;
  int dst = 0;
  for (int y = 0; y < slot->bitmap.rows; ++y) {
    src = startOfLine;
    for (int x = 0; x < slot->bitmap.width; ++x) {
      auto value = *src;
      src++;
      buffer[dst++] = 0xff;
      buffer[dst++] = 0xff;
      buffer[dst++] = 0xff;
      buffer[dst++] = value;
    }
    startOfLine += slot->bitmap.pitch;
  }
  NOT_IMPLEMENTED(ERROR);  // TODO: implement
  return nullptr;          // TODO: atlas->Add(width, height, buffer.data());
}
}  // namespace prt