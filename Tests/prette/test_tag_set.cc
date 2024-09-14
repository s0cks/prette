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

  static inline auto
  Insert(TagSet& tags, const RawTag& value) -> AssertionResult {
    const auto tag = Tag(value);
    const auto pos = tags.insert(tag);
    if(!pos.second)
      return AssertionFailure() << "failed to insert: " << tag;
    return AssertionSuccess();
  }

  static inline auto
  Contains(const TagSet& tags, const RawTag& value) -> AssertionResult {
    const auto tag = Tag(value);
    const auto pos = tags.find(tag);
    if(pos == tags.end())
      return AssertionFailure() << "failed to find: " << tag;
    return AssertionSuccess();
  }

  static inline auto
  Remove(TagSet& tags, const RawTag& value) -> AssertionResult {
    const auto tag = Tag(value);
    const auto removed = tags.erase(tag);
    if(removed != 1)
      return AssertionFailure() << "failed to remove: " << tag;
    return AssertionSuccess();
  }

  TEST_F(TagSetTest, Test) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
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