#include <gtest/gtest.h>
#include "prette/tag.h"

namespace prt {
  using namespace ::testing;

  class TagSetTest : public Test {
  protected:
    TagSetTest() = default;
  public:
    ~TagSetTest() override = default;
  };

  static inline AssertionResult
  Insert(TagSet& tags, const RawTag& value) {
    const auto tag = Tag(value);
    const auto pos = tags.insert(tag);
    if(!pos.second)
      return AssertionFailure() << "failed to insert: " << tag;
    return AssertionSuccess();
  }

  static inline AssertionResult
  Contains(const TagSet& tags, const RawTag& value) {
    const auto tag = Tag(value);
    const auto pos = tags.find(tag);
    if(pos == tags.end())
      return AssertionFailure() << "failed to find: " << tag;
    return AssertionSuccess();
  }

  static inline AssertionResult
  Remove(TagSet& tags, const RawTag& value) {
    const auto tag = Tag(value);
    const auto removed = tags.erase(tag);
    if(removed != 1)
      return AssertionFailure() << "failed to remove: " << tag;
    return AssertionSuccess();
  }

  TEST_F(TagSetTest, Test) {
    TagSet tags;
    ASSERT_TRUE(Insert(tags, "test"));
    ASSERT_TRUE(Insert(tags, "debug"));
    ASSERT_FALSE(Insert(tags, "test"));

    ASSERT_TRUE(Contains(tags, "test"));
    ASSERT_TRUE(Contains(tags, "debug"));
    ASSERT_FALSE(Contains(tags, "invalid"));

    ASSERT_TRUE(Remove(tags, "debug"));

    ASSERT_TRUE(Contains(tags, "test"));
    ASSERT_FALSE(Contains(tags, "debug"));
    ASSERT_FALSE(Contains(tags, "invalid"));
  }
}