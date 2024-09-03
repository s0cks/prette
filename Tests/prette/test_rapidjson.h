#ifndef PRT_TEST_RAPIDJSON_H
#define PRT_TEST_RAPIDJSON_H

#include <gtest/gtest.h>
#include "prette/json.h"

namespace prt {
  class RapidJsonTest : public ::testing::Test {
  protected:
    RapidJsonTest() = default;
  public:
    ~RapidJsonTest() override = default;
  };
}

#endif //PRT_TEST_RAPIDJSON_H