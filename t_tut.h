//
// Created by j.michalczyk on 23.02.2024.
//
#ifndef T_TUT_H
#define T_TUT_H
#include "symbol.h"
#include "deriv.h"

template<double F>
double func1() {
    return F;
}
template<int F>
int func_int() {
    return F;
}

template <unsigned long N>
struct binary {
    static unsigned const value = binary<N/10>::value *2 + N%10;
};

template <>
struct binary<0> {
    static unsigned const value = 0;
};

#endif //T_TUT_H
