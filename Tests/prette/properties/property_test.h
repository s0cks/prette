#ifndef PRT_PROPERTY_TEST_H
#define PRT_PROPERTY_TEST_H

#include <gtest/gtest.h>
#include "prette/properties/property.h"
#include "prette/properties/property_assertions.h"

namespace prt::properties {
  using namespace ::testing;

  class PropertyTest : public Test {
  protected:
    PropertyTest() = default;
  public:
    ~PropertyTest() override = default;
  };
}

#endif //PRT_PROPERTY_TEST_H