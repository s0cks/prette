#include <gtest/gtest.h>
#include "prette/image/image.h"
#include "prette/image/image_format.h"

namespace prt::img {
  using namespace ::testing;

  class ImageTest : public Test {
  protected:
    ImageTest() = default;
  public:
    ~ImageTest() override = default;
  };

  TEST_F(ImageTest, Test_New) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    static const Resolution kImageResolution = Resolution(128, 128);
    const auto img = Image::New(img::kRGBFormat, kImageResolution);
    ASSERT_TRUE(img);
    ASSERT_EQ(img->GetFormat(), img::kRGBFormat);
    ASSERT_EQ(img->GetResolution(), kImageResolution);
    const auto total_size = img::kRGBFormat * kImageResolution;
    ASSERT_EQ(img->GetTotalSize(), total_size);
  }
}