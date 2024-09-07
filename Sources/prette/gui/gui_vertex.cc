#include "prette/gui/gui_vertex.h"

namespace prt::gui {
  Class* Vertex::kClass = nullptr;
  Attribute* Vertex::kPosAttr = nullptr;
  Attribute* Vertex::kUvAttr = nullptr;
  Attribute* Vertex::kColorAttr = nullptr;

  void Vertex::InitClass() {
    PRT_ASSERT(!kClass);
    kClass = Class::New("gui::Vertex");
    PRT_ASSERT(kClass);
    // .pos
    kPosAttr = kClass->CreateVec3fAttr("pos");
    PRT_ASSERT(kPosAttr);
    // .uv
    kUvAttr = kClass->CreateVec2fAttr("uv");
    PRT_ASSERT(kUvAttr);
    // .color
    kColorAttr = kClass->CreateColorAttr("color");
    PRT_ASSERT(kColorAttr);
  }
}