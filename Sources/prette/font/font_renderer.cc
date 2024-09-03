#include "prette/font/font_renderer.h"
#include "prette/program/program_scope.h"

namespace prt::font {
  void FontRenderer::RenderText(const std::string& text, const glm::vec2 pos) {
    float startX = pos[0];
    float startY = pos[1];
    float x = startX;

    program::ApplyProgramScope prog(shader());
    prog.Set("projection", projection_);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(mesh().vao);
    std::string::const_iterator iter;
    for(iter = text.begin(); iter != text.end(); iter++) {
      auto ch = chars()[(*iter)];
      const glm::vec2 pos = {
        scale(x + ch.bearing[0]),
        scale(startY - (ch.size[1] - ch.bearing[1])),
      };
      ch.Draw(pos, mesh());
      x += scale(ch.advance >> 6);
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
}