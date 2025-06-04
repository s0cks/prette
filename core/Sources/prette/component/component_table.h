#ifndef PRT_COMPONENT_TABLE_H
#define PRT_COMPONENT_TABLE_H

#include <gflags/gflags.h>

#include "prette/assertions.h"

namespace prt {
static constexpr const auto kDefaultComponentTableSize = 128;
DECLARE_uint32(component_table_size);

class ComponentTable;
class ComponentTableVisitor {
 protected:
  ComponentTableVisitor() = default;

 public:
  virtual ~ComponentTableVisitor() = default;
  virtual auto Visit(ComponentTable* rhs) -> bool = 0;
};

class ComponentTable {
 public:
  ComponentTable();
  virtual ~ComponentTable();

  auto Accept(ComponentTableVisitor* vis) -> bool {
    ASSERT(vis);
    return vis->Visit(this);
  }
};

template <class C>
class ComponentTableTemplate : public ComponentTable {
 private:
 public:
  ComponentTableTemplate() = default;
  ~ComponentTableTemplate() override = default;
};
}  // namespace prt

#endif  // PRT_COMPONENT_TABLE_H
