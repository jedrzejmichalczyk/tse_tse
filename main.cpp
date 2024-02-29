#include <chrono>
#include <iostream>
#include <cmath>
#include "t_tut.h"
int main() {
    symbol a;
    symbol b;
    symbol c;
    formula f = a*sin(b+c);
    auto t0= std::chrono::high_resolution_clock::now();

    auto sum = 0.0;
    for(int i=0; i<10000000; i++) {
        sum +=  f(a = 3.0,b = 2.0,c=0.3*i );
    }

    auto tk= std::chrono::high_resolution_clock::now();

    std::cout<< std::chrono::duration_cast<std::chrono::milliseconds>(tk-t0)<< std::endl;
    std::cout<<sum<< std::endl;

    t0= std::chrono::high_resolution_clock::now();
    double a_d =3.0;
    double b_d = 2.0;
    double c_d = 0.3;
    double sum_d = 0;
    for(int i=0; i<10000000; i++) {
        sum_d += a_d*sin(b_d + c_d*i);
    }

    tk= std::chrono::high_resolution_clock::now();

    std::cout<< std::chrono::duration_cast<std::chrono::milliseconds>(tk-t0)<< std::endl;
    std::cout<<sum_d<< std::endl;

    return 0;
}
