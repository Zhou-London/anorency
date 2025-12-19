#include <gtest/gtest.h>

#include "version.h"

TEST(Info, info) { EXPECT_EQ(version(), "v0.1"); }
