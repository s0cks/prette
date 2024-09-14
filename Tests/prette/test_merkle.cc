#include <gtest/gtest.h>
#include "prette/merkle.h"

namespace prt::merkle {
  using namespace ::testing;

  class MerkleTreeTest : public Test {
  protected:
    MerkleTreeTest() = default;
  public:
    ~MerkleTreeTest() override = default;
  };

  TEST_F(MerkleTreeTest, Test_0Leaves) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    static const std::vector<uint256> kTestLeaves = {};
    Tree tree(kTestLeaves);
    ASSERT_TRUE(tree.IsEmpty());
  }

  TEST_F(MerkleTreeTest, Test_1Leaves) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    static const uint256 kLeaf1 = sha256::Nonce();
    static const std::vector<uint256> kTestLeaves = {
      kLeaf1,
    };
    static const auto kExpectedRoot = sha256::Concat(kLeaf1, kLeaf1);

    Tree tree(kTestLeaves);
    ASSERT_FALSE(tree.IsEmpty());
    ASSERT_EQ(tree.GetRootHash(), kExpectedRoot);
  }

  TEST_F(MerkleTreeTest, Test_2Leaves) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    static const uint256 kLeaf1 = sha256::Nonce();
    static const uint256 kLeaf2 = sha256::Nonce();
    static const std::vector<uint256> kTestLeaves = {
      kLeaf1, kLeaf2
    };
    static const auto kExpectedRoot = sha256::Concat(kLeaf1, kLeaf2);

    Tree tree(kTestLeaves);
    ASSERT_FALSE(tree.IsEmpty());
    ASSERT_EQ(tree.GetRootHash(), kExpectedRoot);
  }

  TEST_F(MerkleTreeTest, Test_3Leaves) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    static const uint256 kLeaf1 = sha256::Nonce();
    static const uint256 kLeaf2 = sha256::Nonce();
    static const uint256 kLeaf3 = sha256::Nonce();
    static const std::vector<uint256> kTestLeaves = {
      kLeaf1, kLeaf2, kLeaf3,
    };
    static const auto kExpectedRoot = sha256::Concat(sha256::Concat(kLeaf1, kLeaf2), sha256::Concat(kLeaf3, kLeaf3));

    Tree tree(kTestLeaves);
    ASSERT_FALSE(tree.IsEmpty());
    ASSERT_EQ(tree.GetRootHash(), kExpectedRoot);
  }

  TEST_F(MerkleTreeTest, Test_4Leaves) { // NOLINT(modernize-use-trailing-return-type,cppcoreguidelines-avoid-non-const-global-variables)
    static const uint256 kLeaf1 = sha256::Nonce();
    static const uint256 kLeaf2 = sha256::Nonce();
    static const uint256 kLeaf3 = sha256::Nonce();
    static const uint256 kLeaf4 = sha256::Nonce();
    static const std::vector<uint256> kTestLeaves = {
      kLeaf1, kLeaf2, kLeaf3, kLeaf4,
    };
    static const auto kExpectedRoot = sha256::Concat(sha256::Concat(kLeaf1, kLeaf2), sha256::Concat(kLeaf3, kLeaf4));

    Tree tree(kTestLeaves);
    ASSERT_FALSE(tree.IsEmpty());
    ASSERT_EQ(tree.GetRootHash(), kExpectedRoot);
  }
}