#ifndef PRT_INPUT_H
#define PRT_INPUT_H

#include <vector>
#include <ostream>
#include "prette/uuid.h"
#include "prette/common.h"

namespace prt {
  namespace window {
    class Window;
  }

  class Input {
  protected:
    UUID id_;

    Input() = default;

    inline void SetId(const UUID& id) {
      id_ = id;
    }
  public:
    virtual ~Input() = default;

    const UUID& GetId() const {
      return id_;
    }

    virtual bool Equals(Input* rhs) const {
      return GetId() == rhs->GetId();
    }

    virtual std::string ToString() const = 0;
  };

  typedef std::vector<Input*> InputList;
}

#endif //PRT_INPUT_H