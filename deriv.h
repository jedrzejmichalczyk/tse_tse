#ifndef DERIV_H
#define DERIV_H

#include "symbol.h"
#include "op.h"
template <typename F ,typename  b>
struct Derivative {
    typedef std::conditional_t<std::is_same_v<F,b>,One,Zero> derivType;
};

template <typename   F ,typename  b>
struct Derivative<UnaryOp<sin_symbol,F>,b> {
    using _dF = typename Derivative<F,b>::derivType;
    typedef BinaryOp<multiply_symbol,_dF,UnaryOp<cos_symbol,F>> derivType;
};

template <typename   F ,typename  b>
struct Derivative<UnaryOp<cos_symbol,F>,b> {
    using _dF = typename Derivative<F,b>::derivType;
    using v = BinaryOp<multiply_symbol,_dF,UnaryOp<sin_symbol,F>>;
    typedef UnaryOp<negate_symbol,v> derivType;
};

template <typename   F ,typename  b>
struct Derivative<UnaryOp<negate_symbol,F>,b> {
    using _dF = typename Derivative<F,b>::derivType;
    typedef UnaryOp<negate_symbol,_dF> derivType;
};

template <typename   L,typename  R ,typename  b>
struct Derivative<BinaryOp<plus_symbol,L,R>,b> {
    using _dF_l = typename Derivative<L,b>::derivType;
    using _dF_r = typename Derivative<R,b>::derivType;
    typedef BinaryOp<plus_symbol,_dF_l,_dF_r> derivType;

};

template <typename   L,typename   R ,typename  b>
struct Derivative<BinaryOp<multiply_symbol,L,R>,b> {
    using _dF_l = typename Derivative<L,b>::derivType;
    using _dF_r = typename Derivative<R,b>::derivType;
    using _dF_l_r = BinaryOp<multiply_symbol,_dF_l,R>;
    using _dF_r_l = BinaryOp<multiply_symbol,L,_dF_r>;
    typedef BinaryOp<plus_symbol,_dF_l_r,_dF_r_l> derivType;

};

template <typename   up, typename   down ,typename  b>
struct Derivative<BinaryOp<devides_symbol,up,down>,b> {
    using _dF_up = typename Derivative<up,b>::derivType;
    using _dF_down = typename Derivative<down,b>::derivType;
    using _dFup_down = BinaryOp<multiply_symbol,_dF_up,down>;
    using _dFdown_up = BinaryOp<multiply_symbol,_dF_down,up>;
    using _d_up = BinaryOp<minus_symbol,_dFup_down,_dFdown_up>;
    using _d_down = BinaryOp<multiply_symbol,down,down>;
    typedef BinaryOp<devides_symbol,_d_up,_d_down> derivType;

};
// template <typename  F,symbol b, int N>
// struct DerN {
//     using inner =  decltype(Derivative<F,b>::derivType);
//     typename decltype(DerN<inner,b,N-1>::derivType) derivType;
// };
//
// template <typename  F, symbol b>
// struct DerN<F,b,1> : Derivative<F,b> {
// };



template<typename Expr, typename Arg>
auto derivative(const Expr, const Arg) {
    using result = typename Derivative<Expr,Arg>::derivType;
    return result();
}
// template<typename Expr, typename Arg>
// auto derivative(const Expr, const Arg x, int N) {
//     return DerN<Expr,x,N>().derivType;
// }

#endif //DERIV_H