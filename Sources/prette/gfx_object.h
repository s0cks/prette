#ifndef PRT_GFX_OBJECT_H
#define PRT_GFX_OBJECT_H

#include "prette/gfx.h"
#include "prette/object.h"

namespace prt::gfx {
  template<typename Id>
  class Object : public prt::Object {
  protected:
    Id id_;

    Object() = default;
    explicit Object(const Id id):
      prt::Object(),
      id_(id) {
    }
  public:
    ~Object() override = default;

    Id GetId() const {
      return id_;
    }

    bool IsValid() const {
      return (bool) id_;
    }

    bool IsInvalid() const {
      return !IsValid();
    }
  };
}

#endif //PRT_GFX_OBJECT_H