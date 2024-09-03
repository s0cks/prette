#ifndef PRT_GUI_CONTEXT_H
#define PRT_GUI_CONTEXT_H

#include "prette/shape.h"
#include "prette/vao/vao.h"
#include "prette/vbo/vbo.h"
#include "prette/ibo/ibo.h"
#include "prette/gui/gui_vertex.h"

namespace prt::gui {
  class Context {
  protected:
    Vao* vao_;
    Vbo* vbo_;
    Ibo* ibo_;
    Color color_;

    bool dirty_;

    uword max_vertices_;
    uword max_indices_;

    VertexList vertices_;
    std::vector<uint32_t> indices_;

    inline void SetDirty(const bool value = true) {
      dirty_ = value;
    }
  public:
    Context(const uword num_vertices = 512, const uword num_indices = 512);
    ~Context();

    Vao* GetVao() const {
      return vao_;
    }

    Vbo* GetVbo() const {
      return vbo_;
    }

    Ibo* GetIbo() const {
      return ibo_;
    }

    bool IsDirty() const {
      return dirty_;
    }

    void SetColor(const Color& c) {
      color_ = c;
    }

    const Color& GetColor() const {
      return color_;
    }

    const gui::VertexList& GetVertices() const {
      return vertices_;
    }

    void Commit();
    void DrawRect(const glm::mat4& transform, const Rectangle& rect, const float zLevel = 0.0f);
    void DrawTexturedRect(const glm::mat4& transform, const Rectangle& rect, const float zLevel = 0.0f);

    void Reset() {
      vertices_.clear();
      indices_.clear();
    }
  };
}

#endif //PRT_GUI_CONTEXT_H