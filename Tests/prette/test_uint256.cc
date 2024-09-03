#include <gtest/gtest.h>
#include "prette/sha256.h"
#include "prette/uint256.h"

namespace prt {
  using namespace ::testing;

  class UInt256Test : public Test {
  protected:
    UInt256Test() = default;
  public:
    ~UInt256Test() override = default;
  };

  TEST_F(UInt256Test, Test_SizeInBits) {
    ASSERT_EQ(uint256::kSizeInBits, 256);
  }

  TEST_F(UInt256Test, Test_SizeInBytes) {
    ASSERT_EQ(uint256::kSizeInBytes, 256 / kBitsPerByte);
  }

  TEST_F(UInt256Test, Test_SizeInWords) {
    ASSERT_EQ(uint256::kSizeInWords, 256 / kBitsPerWord);
  }

  TEST_F(UInt256Test, Test_Hash) {
    const auto nonce = sha256::Nonce(4096);
    DLOG(INFO) << "nonce: ";
    DLOG(INFO) << " - hex: " << nonce.ToHexString();
    DLOG(INFO) << " - binary: " << nonce.ToBinaryString();
  }
}