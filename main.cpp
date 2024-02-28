#include <chrono>
#include <iostream>
#include "t_tut.h"
using namespace std;
// template<typename f, typename R, symbol ... as>
// struct f_help {
//     std::vector<R> v{as};
//     double f_v(vector<double>& x) {
//         return 0;
//     }
// };

int main() {
    symbol a;
    symbol b;
    symbol c;
    constant_symbol<0.3> e;
    constant_symbol<2> c2;
    constant_symbol<3> c3;
    auto w = 0.3 + 0.5;
    constexpr double cc = 0.3;
    // formula f = derivative(a*a,a) ;

    // formula f = (a*a*a);
    formula f = a*b;
    // formula f_alt = Derivative<decltype(a*sin(b + c)),c>::derivType;
    // formula f1 = DerN<decltype(a*sin(b + c)),c,1>().derivType;
    // formula f7 = DerN<decltype(a*sin(b + c)),c,5>().derivType;

    auto t0= std::chrono::high_resolution_clock::now();

    auto sum = 0.0;
    for(int i=0; i<10000000; i++) {

        sum =  f(a = 3.0*i,b = 3.0 );
        // sum+=  f(vector<double>{3.0*i,2, 0.3},{a,b,c});
    }

    auto tk= std::chrono::high_resolution_clock::now();

    std::cout<<chrono::duration_cast<chrono::milliseconds>(tk-t0)<<endl;
    std::cout<<sum<<endl;

    t0= std::chrono::high_resolution_clock::now();
    double a_d =3.0;
    double b_d = 2.0;
    double c_d = 0.3;
    double sum_d = 0;
    for(int i=0; i<10000000; i++) {
        // sum += a_d*cos(b_d + c_d*i);
        auto v = a_d*i;
        // sum += 3*pow(a_d *i,2.0);
        sum_d += 4*v*v*v;
    }

    tk= std::chrono::high_resolution_clock::now();

    std::cout<<chrono::duration_cast<chrono::milliseconds>(tk-t0)<<endl;
    std::cout<<sum_d<<endl;

    return 0;
}
