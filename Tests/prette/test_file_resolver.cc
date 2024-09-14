#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "prette/flags.h"
#include "prette/file_resolver.h"
#include "prette/mock_subscription.h"

namespace prt {
  using namespace ::testing;

  class FileResolverTest : public Test {
  protected:
    FileResolverTest() = default;
  public:
    ~FileResolverTest() override = default;
  };

  TEST_F(FileResolverTest, Test_ListAllInDirectory) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    rx::MockSubscription<std::string> sub;
    EXPECT_CALL(sub, OnNext(Eq(fmt::format("{0:s}/fonts", FLAGS_resources))));
    EXPECT_CALL(sub, OnNext(Eq(fmt::format("{0:s}/materials", FLAGS_resources))));
    EXPECT_CALL(sub, OnNext(Eq(fmt::format("{0:s}/meshes", FLAGS_resources))));
    EXPECT_CALL(sub, OnNext(Eq(fmt::format("{0:s}/shaders", FLAGS_resources))));
    EXPECT_CALL(sub, OnNext(Eq(fmt::format("{0:s}/textures", FLAGS_resources))));
    EXPECT_CALL(sub, OnNext(Eq(fmt::format("{0:s}/.gitignore", FLAGS_resources))));
    EXPECT_CALL(sub, OnNext(Eq(fmt::format("{0:s}/.gitkeep", FLAGS_resources))));
    const auto observable = fs::ListAllInDirectory(FLAGS_resources)
      .map(fs::ToPath);
    const auto subscription = sub.Subscribe(observable);
  }

  TEST_F(FileResolverTest, Test_ListFilesInDirectory) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    rx::MockSubscription<std::string> sub;
    EXPECT_CALL(sub, OnNext(Eq(fmt::format("{0:s}/.gitignore", FLAGS_resources))));
    EXPECT_CALL(sub, OnNext(Eq(fmt::format("{0:s}/.gitkeep", FLAGS_resources))));
    const auto observable = fs::ListFilesInDirectory(FLAGS_resources)
      .map(fs::ToPath);
    const auto subscription = sub.Subscribe(observable);
  }

  TEST_F(FileResolverTest, Test_ListDirsInDirectory) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    rx::MockSubscription<std::string> sub;
    EXPECT_CALL(sub, OnNext(Eq(fmt::format("{0:s}/fonts", FLAGS_resources))));
    EXPECT_CALL(sub, OnNext(Eq(fmt::format("{0:s}/materials", FLAGS_resources))));
    EXPECT_CALL(sub, OnNext(Eq(fmt::format("{0:s}/meshes", FLAGS_resources))));
    EXPECT_CALL(sub, OnNext(Eq(fmt::format("{0:s}/shaders", FLAGS_resources))));
    EXPECT_CALL(sub, OnNext(Eq(fmt::format("{0:s}/textures", FLAGS_resources))));
    const auto observable = fs::ListDirsInDirectory(FLAGS_resources)
      .map(fs::ToPath);
    const auto subscription = sub.Subscribe(observable);
  }
}