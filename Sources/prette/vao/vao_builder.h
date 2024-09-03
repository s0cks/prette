#ifndef PRT_VAO_BUILDER_H
#define PRT_VAO_BUILDER_H

#include "prette/rx.h"
#include "prette/builder.h"
#include "prette/vao/vao_id.h"

namespace prt::vao {
  class Vao;
  class VaoBuilder : public BuilderBase {
  protected:
    VaoId id_;
  public:
    explicit VaoBuilder(const VaoId id = kInvalidVaoId):
      BuilderBase(),
      id_(id) {
    }
    ~VaoBuilder() override = default;
    
    Vao* Build() const;
    rx::observable<Vao*> BuildAsync() const;
  };
}

#endif //PRT_VAO_BUILDER_H