#ifndef PRT_GUI_VERTEX_H
#define PRT_GUI_VERTEX_H

#include "prette/color.h"
#include "prette/class.h"

namespace prt::gui {
  struct Vertex {
    glm::vec3 pos;
    glm::vec2 uv;
    Color color;
    
    friend std::ostream& operator<<(std::ostream& stream, const Vertex& rhs) {
      stream << "gui::Vertex(";
      stream << "pos=" << glm::to_string(rhs.pos) << ", ";
      stream << "uv=" << glm::to_string(rhs.uv) << ", ";
      stream << "color=" << glm::to_string(rhs.color);
      stream << ")";
      return stream;
    }
  public:
    static Attribute* kPosAttr;
    static Attribute* kUvAttr;
    static Attribute* kColorAttr;
    static Class* kClass;
    static void InitClass();
  };

  typedef std::vector<Vertex> VertexList;
}

#endif //PRT_GUI_VERTEX_H