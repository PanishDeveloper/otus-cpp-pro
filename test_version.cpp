#include <gtest/gtest.h>
#include "lib.h"

TEST(TestVersion, ValidVerson)
{
    EXPECT_GT(version(), 0);
}
