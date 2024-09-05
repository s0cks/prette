#ifndef PRT_VBO_VISITOR_H
#define PRT_VBO_VISITOR_H

namespace prt::vbo {
  class Vbo;
  class VboVisitor {
  protected:
    VboVisitor() = default;
  public:
    virtual ~VboVisitor() = default;
    virtual bool VisitVbo(Vbo* vbo) = 0;
  };
}

#endif //PRT_VBO_VISITOR_H