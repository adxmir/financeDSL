#include <gtest/gtest.h>
#include "include/financeDsl.hpp"

TEST(Currency, codeReturn){
    Money<USD> transaction1(23.5050);
    Money<GBP> transaction2 = convert<USD,GBP>(transaction1);

    Money<JPY> transaction3(50000.123456789);
    Money<GBP> transaction4 = convert<JPY,GBP>(transaction3);
    EXPECT_NEAR(double(transaction1), 23.505, 0.001);
    EXPECT_NEAR(double(transaction4), 263.00064, 0.00001);
}

TEST(Currency, codeAddition){
    Money<USD> m1(25.500);
    Money<USD> m2(25.500);

    Money<GBP> m3(103.230955);
    Money<GBP> m4(252300);

    m1 = m1 + m2;
    EXPECT_EQ(double(m1), 51);

    m3 = m3 + m4;
    std::ostringstream s;
    s << m3;
    ASSERT_STREQ(s.str().c_str(), "252403.230955");
}