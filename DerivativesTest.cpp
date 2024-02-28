//
// Created by j.michalczyk on 23.06.2021.
//

#include <gtest/gtest.h>
#include <iostream>
#include <valarray>
#include "t_tut.h"
using namespace std;

TEST(t_tut, symbol_a) {
    symbol a;
    symbol b;

    formula check = derivative(a  + b,a);
    cout<<check(a = 0.3,b = 0)<<endl;
    cout<<1.0<<endl;
    ASSERT_EQ(check(a = 0.3,b = 0),1.0);

}

TEST(t_tut, symbol_2a) {
    symbol a;
    symbol b;

    formula check = derivative(a  + a,a);
    cout<<check(a = 0.3,b = 0)<<endl;
    cout<<2.0<<endl;
    ASSERT_EQ(check(a = 0.3,b = 0),2.0);

}

TEST(t_tut, symbol_ab) {
    symbol a;
    symbol b;

    formula check = derivative(a*b,a); // -b/(a^2)
    formula actual = b;
    std::cout<<check(a = 0.3,b = 3.0)<<std::endl;
    std::cout<<actual(a=0.3,b=3.0)<<std::endl;
    ASSERT_EQ(check(a = 0.3,b = 3.0),actual(a = 0.3,b = 3.0));

}

TEST(t_tut, symbol_bdiva) {
    symbol a;
    symbol b;

    formula check = derivative(b/a,a); // -b/(a^2)
    formula actual = -b/(a*a);
    cout<<check(a = 0.3,b = 3.0)<<endl;
    cout<<actual(a=0.3,b=3.0)<<endl;
    ASSERT_EQ(check(a = 0.3,b = 3.0),actual(a = 0.3,b = 3.0));

}

TEST(t_tut, symbol_power) {
    symbol a;
    symbol b;
    constant_symbol<0.3> c;
    formula check = derivative(b/a + c,a); // -b/(a^2)
    formula actual = -b/(a*a);
    cout<<check(a = 0.3,b = 3.0)<<endl;
    cout<<actual(a=0.3,b=3.0)<<endl;
    ASSERT_EQ(check(a = 0.3,b = 3.0),actual(a = 0.3,b = 3.0));

}



