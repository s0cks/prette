#include "prette/font/glyph.h"
#include "prette/font/font.h"

namespace prt::font {
  static inline void
  GenerateGlyphTexture(TextureId& texture, FT_Bitmap& bitmap) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, bitmap.width, bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap.buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }

  Glyph::Glyph(FT_Bitmap& bitmap, FT_Int left, FT_Int top, FT_Vector& advance):
    texture(),
    size(glm::ivec2(bitmap.width, bitmap.rows)),
    bearing(glm::ivec2(left, top)),
    advance(static_cast<GLuint>(advance.x)) {
    GenerateGlyphTexture(texture, bitmap);
  }

#define X(Pos) (pos[0])
#define Y(Pos) (pos[1])

  void Glyph::Draw(const glm::vec2 pos, FontMesh& mesh) {
    float vertices[6][4] = {
      { X(pos),             Y(pos) + size[1],       0.0f, 0.0f },
      { X(pos),             Y(pos),                 0.0f, 1.0f },
      { X(pos) + size[0],   Y(pos),                 1.0f, 1.0f },

      { X(pos),             Y(pos) + size[1],       0.0f, 0.0f },
      { X(pos) + size[0],   Y(pos),                 1.0f, 1.0f },
      { X(pos) + size[0],   Y(pos) + size[1],       1.0f, 0.0f },
    };
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }
}