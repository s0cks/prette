#include <gtest/gtest.h>
#include "prette/class.h"

namespace prt {
  using namespace ::testing;

  class ClassTest : public Test {
  protected:
    ClassTest() = default;
  public:
    ~ClassTest() override = default;
  };

  TEST_F(ClassTest, Test_GetAllocationSize) {
    // byte (8 bits)
    ASSERT_EQ(Class::kByte->GetAllocationSize(), sizeof(int8_t));
    ASSERT_EQ(Class::kUnsignedByte->GetAllocationSize(), sizeof(uint8_t));
    // short (16 bits)
    ASSERT_EQ(Class::kShort->GetAllocationSize(), sizeof(int16_t));
    ASSERT_EQ(Class::kUnsignedShort->GetAllocationSize(), sizeof(uint16_t));
    // int (32 bits)
    ASSERT_EQ(Class::kInt->GetAllocationSize(), sizeof(int32_t));
    ASSERT_EQ(Class::kUnsignedInt->GetAllocationSize(), sizeof(uint32_t));
    // float (32 bits)
    ASSERT_EQ(Class::kFloat->GetAllocationSize(), sizeof(float));
  }
}