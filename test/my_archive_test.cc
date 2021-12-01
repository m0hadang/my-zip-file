#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../my_archive.h"

using namespace mohadangkim;

TEST(MY_ARCHIVE, Add) {
  EXPECT_EQ(Add(1,2), 3);
}