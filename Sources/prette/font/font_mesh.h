#ifndef PRT_FONT_MESH_H
#define PRT_FONT_MESH_H

#include "prette/gfx.h"

namespace prt::font {
  struct FontMesh {
  public:
    GLuint vao;
    GLuint vbo;

    FontMesh();
    ~FontMesh() = default;
  };
}

#endif //PRT_FONT_MESH_H