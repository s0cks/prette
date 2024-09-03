#include "prette/font/font_mesh.h"

namespace prt::font {
  static constexpr const int8_t kVertexLength = 4;
  static constexpr const uint64_t kVertexSize = sizeof(float) * kVertexLength;
  
  static constexpr const int8_t kNumberOfVertices = 6;
  static constexpr const uint64_t kFontMeshSize = kNumberOfVertices * kVertexSize;

  FontMesh::FontMesh():
    vao(0),
    vbo(0) {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, kFontMeshSize, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, kVertexLength, GL_FLOAT, GL_FALSE, kVertexSize, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }
}