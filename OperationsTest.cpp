//
// Created by j.michalczyk on 23.06.2021.
//

#include <chrono>
#include <gtest/gtest.h>
#include <iostream>
#include <cmath>
#include "t_tut.h"

using namespace std;

TEST(op, binary) {
    std::cout<<binary<101010>::value<<endl;
}

TEST(op, sin_formula) {
    symbol a;
    symbol b;
    symbol c;
    formula f = a*sin(b + c);

    std::cout<<f(a=1,b=0,c=0.5*numbers::pi)<<std::endl;
    GTEST_ASSERT_EQ(f(a=1,b=0,c=0.5*numbers::pi),1.0);

}


TEST(op, symbol_add) {
    symbol a;
    double result = func1<0.3>();

}
// template <class U>
//     requires std::is_convertible_v<U&&, requalify_as_const_t<remove_rvalue_reference_t<T>>>

TEST(op, use_as_func) {
    symbol a;
    formula s = a+a;
    function<double(double)> f = [&s, &a](double x){return s(a=std::move(x));};

    double arg = 3.0;
    std::cout<<f(arg)<<std::endl;
    ASSERT_EQ(f(arg),2*3.0);
}