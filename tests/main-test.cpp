#include <gtest/gtest.h>
#include "include/financeDsl.hpp"

TEST(Conversion, codeReturn){
    Money<GBP> transaction1(23.5050);
    Money<USD> transaction2 = convert<USD,GBP>(transaction1);
    EXPECT_NEAR(double(transaction1), 23.505, 0.1);
}