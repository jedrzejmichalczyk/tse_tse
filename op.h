//
// Created by j.michalczyk on 28.02.2024.
//

#ifndef OP_H
#define OP_H
#include "symbol.h"
template<typename Op,typename T>
struct UnaryOp : symbolic_expression<Op,T> {
    T expr_;
    template<class Arg>
    constexpr auto operator()(Arg&& arg) const{
        return Op::apply(expr_(arg));
    }
};

template <typename  Op,typename  T>
struct is_symbolic<UnaryOp<Op,T>>: std::true_type {};


template<typename  Op, typename   L, typename  R>
struct BinaryOp : symbolic_expression<Op,L,R> {
    L left_;
    R right_;
    template<class Arg>
    constexpr auto operator()(Arg&& arg) const{
        return Op::apply(left_(arg),right_(arg));
    }
};


template <typename  Op,typename  Lhs, typename  Rhs>
struct is_symbolic<BinaryOp<Op,Lhs,Rhs>>: std::true_type {};

struct plus_symbol {

    template <class Lhs, class Rhs>
    static constexpr auto apply(Lhs L, Rhs R){
        return L + R;
    }

};


struct minus_symbol {
    template <class Lhs, class Rhs>
    static constexpr auto apply(Lhs L, Rhs R){
        return L-R;
    }

};
struct multiply_symbol {

    template <class Lhs, class Rhs>
    static constexpr auto apply(Lhs L, Rhs R){
        return L*R;
    }
};

struct devides_symbol {
    template <class Lhs, class Rhs>
    static constexpr auto apply(Lhs L, Rhs R){
        return L/R;
    }
};
struct sin_symbol {
    template <class Arg>
    static constexpr auto apply(Arg&& arg) {
        return sin(std::forward<Arg>(arg));
    }
};

struct negate_symbol {
    template <class Arg>
    static constexpr auto apply(Arg&& arg) {
        return -(std::forward<Arg>(arg));
    }

};

struct cos_symbol {
    template <class Arg>
    static constexpr auto apply(Arg&& arg) {
        return cos(std::forward<Arg>(arg));
    }
};

template <symbolic Lhs, symbolic Rhs>
constexpr BinaryOp<plus_symbol, Lhs, Rhs> operator+(Lhs,Rhs){return {};}

template <symbolic Lhs, symbolic Rhs>
constexpr BinaryOp<minus_symbol, Lhs, Rhs> operator-(Lhs,Rhs){return {};}

template <symbolic Lhs, symbolic Rhs>
constexpr BinaryOp<multiply_symbol, Lhs, Rhs> operator*(Lhs,Rhs){return {};}

template <symbolic Lhs, symbolic Rhs>
constexpr BinaryOp<devides_symbol, Lhs, Rhs> operator/(Lhs,Rhs){return {};}

template<symbolic Arg>
constexpr UnaryOp<sin_symbol,Arg> sin(Arg) noexcept{return {};}
template<symbolic Arg>
constexpr UnaryOp<cos_symbol,Arg> cos(Arg) noexcept{return {};}
template<symbolic Arg>
constexpr UnaryOp<negate_symbol,Arg> operator-(Arg) noexcept{return {};}

#endif //OP_H
