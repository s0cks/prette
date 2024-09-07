#ifndef PRT_OBJECT_H
#define PRT_OBJECT_H

#include <string>
#include <utility>
#include "prette/metadata.h"

namespace prt {
  class Object {
    DEFINE_NON_COPYABLE_TYPE(Object);
  private:
    Metadata meta_{};
  protected:
    Object() = default;
    explicit Object(Metadata meta):
      meta_(std::move(meta)) {  
    }

    void SetMeta(const Metadata& rhs) {
      meta_ = rhs;
    }
  public:
    virtual ~Object() = default;

    auto GetMeta() const -> const Metadata& {
      return meta_;
    }

    virtual auto ToString() const -> std::string = 0;
  };
}

#endif //PRT_OBJECT_H