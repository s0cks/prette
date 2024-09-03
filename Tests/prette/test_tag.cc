#include <gtest/gtest.h>
#include "prette/tag.h"

namespace prt {
  using namespace ::testing;

  class TagTest : public Test {
  protected:
    TagTest() = default;
  public:
    ~TagTest() override = default;
  };

  static inline AssertionResult
  IsTag(const Tag& actual, const std::string& expected) {
    if(actual != expected)
      return AssertionFailure() << "expected " << actual << " to equal: " << expected;
    return AssertionSuccess();
  }

  TEST_F(TagTest, Test) {
    Tag t1("test");
    ASSERT_TRUE(IsTag(t1, "test"));

    Tag t2("debug");
    ASSERT_TRUE(IsTag(t2, "debug"));

    Tag t3("test3");
    ASSERT_TRUE(IsTag(t3, "test3"));

    Tag t4("test");
    ASSERT_TRUE(IsTag(t4, "test"));

    ASSERT_EQ(t1, t1);
    ASSERT_NE(t1, t2);
    ASSERT_NE(t1, t3);
    ASSERT_EQ(t1, t4);
  }
}