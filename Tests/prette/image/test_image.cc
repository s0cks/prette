#include <gtest/gtest.h>
#include "prette/image/image.h"

namespace prt::img {
  using namespace ::testing;

  class ImageTest : public Test {
  protected:
    ImageTest() = default;
  public:
    ~ImageTest() override = default;
  };

  TEST_F(ImageTest, Test_New) {
    static constexpr const ImageSize kImageSize(128);
    const auto img = Image::New(img::kRGB, kImageSize, kImageSize[0] * kImageSize[1] * 3);
    ASSERT_TRUE(img);
    ASSERT_EQ(img->format(), img::kRGB);
    ASSERT_EQ(img->size(), kImageSize);
    ASSERT_EQ(img->GetNumberOfBytes(), sizeof(uint8_t) * 3 * kImageSize[0] * kImageSize[1]);
    const auto expected_total_size = sizeof(Image)
      + sizeof(uword)                                     // format
      + (sizeof(uword) * 2)                               // size
      + sizeof(uint8_t) * img->GetNumberOfBytes();        // data
    ASSERT_EQ(img->GetTotalNumberOfBytes(), expected_total_size);
  }
}