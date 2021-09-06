#include "project/MicrobenchMemory.hpp"

#include <gtest/gtest.h>

TEST(TmpAddTest, CheckValues) {
  ASSERT_EQ(MicrobenchMemory::foo(1), 2);
  EXPECT_TRUE(true);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
