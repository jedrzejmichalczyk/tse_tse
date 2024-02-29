//
// Created by j.michalczyk on 23.06.2021.
//

#include <chrono>
#include <gtest/gtest.h>
#include <iostream>
#include <cmath>
#include "t_tut.h"

using namespace std;

TEST(t_tut, binary) {
    std::cout<<binary<101010>::value<<endl;
}

TEST(t_tut, sin_formula) {
    symbol a;
    symbol b;
    symbol c;
    formula f = a*sin(b + c);

    std::cout<<f(a=1,b=0,c=0.5*numbers::pi)<<std::endl;
    GTEST_ASSERT_EQ(f(a=1,b=0,c=0.5*numbers::pi),1.0);

}


TEST(t_tut, symbol_add) {
    symbol a;
    double result = func1<0.3>();

}


TEST(t_tut, use_as_func) {
    symbol Q;
    formula s = Q;
    function<double(double)> f = [&s, &Q](const double x){return s(Q=0.3);};

    cout<<f(3)<<endl;
    // Eigen::MatrixXd m(2,2);
    // using r = requalify_as_const_t<remove_rvalue_reference_t<decltype(m)>>;
    // constexpr double m = 3.0;
    // cout<<s(Q = Eigen::MatrixXd(1,2,1))<<endl;
}