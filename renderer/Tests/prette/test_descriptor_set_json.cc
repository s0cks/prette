#include <gtest/gtest.h>

namespace prt::json {
using namespace ::testing;

class DescriptorSetHandlerTest : public Test {
 public:
  DescriptorSetHandlerTest() = default;
  ~DescriptorSetHandlerTest() override = default;
};

TEST_F(DescriptorSetHandlerTest, Test_FromJsonFile) {}
}  // namespace prt::json