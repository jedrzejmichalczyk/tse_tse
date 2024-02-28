//
// Created by j.michalczyk on 23.06.2021.
//

#include <chrono>
#include <gtest/gtest.h>
#include <iostream>
#include <valarray>
#include "t_tut.h"
using namespace std;

TEST(t_tut, binary) {
    std::cout<<binary<101010>::value<<endl;
}

TEST(t_tut, symbol_id) {
    symbol a;
    symbol b;
    symbol c;
    formula f = a*sin(b + c);

    auto t0= std::chrono::high_resolution_clock::now();

    double sum = 0;
    for(int i=0; i<10000000; i++) {
        sum+=  f(a = 3,b=2,c=0.3*i);
    }

    auto tk= std::chrono::high_resolution_clock::now();

    std::cout<<chrono::duration_cast<chrono::milliseconds>(tk-t0)<<endl;
    std::cout<<sum<<endl;

    t0= std::chrono::high_resolution_clock::now();
    double a_d =3.0;
    double b_d = 2.0;
    double c_d = 0.3;
    sum = 0;
    for(int i=0; i<10000000; i++) {
        sum += a_d*sin(b_d + c_d*i);
    }

    tk= std::chrono::high_resolution_clock::now();

    std::cout<<chrono::duration_cast<chrono::milliseconds>(tk-t0)<<endl;
    std::cout<<sum<<endl;



}


TEST(t_tut, symbol_add) {
    symbol a;
    double result = func1<0.3>();

}


