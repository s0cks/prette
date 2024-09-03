#ifndef PRT_OBJECT_H
#define PRT_OBJECT_H

#include <string>
#include "prette/metadata.h"

namespace prt {
  class Object {
  protected:
    Metadata meta_;

    Object() = default;
    explicit Object(const Metadata& meta):
      meta_(meta) {  
    }

    void SetMeta(const Metadata& rhs) {
      meta_ = rhs;
    }
  public:
    virtual ~Object() = default;

    const Metadata& GetMeta() const {
      return meta_;
    }

    virtual std::string ToString() const = 0;
  };
}

#endif //PRT_OBJECT_H