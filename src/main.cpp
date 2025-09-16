#include <iostream>
#include "include/financeDsl.hpp"

int main(){

    Money<GBP> a1(250);
    auto a2 = 32.45_GBP;
    a2 = a1 + a2;

    a2 = a2 * 5;

    std::cout << a2 << "\n";

    return 0;
}