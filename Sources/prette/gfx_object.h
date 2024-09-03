#ifndef PRT_GFX_OBJECT_H
#define PRT_GFX_OBJECT_H

#include "prette/gfx.h"
#include "prette/object.h"

namespace prt::gfx {
  class Object : public prt::Object {
  protected:
    Object() = default;
  public:
    ~Object() override = default;
  };

  template<typename Id>
  class ObjectTemplate : public Object {
  protected:
    Id id_;

    ObjectTemplate() = default;
    explicit ObjectTemplate(const Id id):
      Object(),
      id_(id) {
    }
  public:
    ~ObjectTemplate() override = default;

    Id GetId() const {
      return (Id) id_;
    }
  };
}

#endif //PRT_GFX_OBJECT_H