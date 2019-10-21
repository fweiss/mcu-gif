#include "gd.h"
#include "gd_internal.h"

#include "gtest/gtest.h"
#include "fff.h"

// do in main
//DEFINE_FFF_GLOBALS;

class Broom : public ::testing::Test {
protected:
};

TEST_F(Broom, one) {
    EXPECT_EQ(1, 1);
}
