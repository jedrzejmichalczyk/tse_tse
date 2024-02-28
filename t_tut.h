//
// Created by j.michalczyk on 23.02.2024.
//
#ifndef T_TUT_H
#define T_TUT_H
#include <functional>
#include <cmath>
#include <valarray>

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

template <class T>
struct remove_lvalue_reference: std::type_identity<T>{};

template <class T>
requires std::is_lvalue_reference_v<T>
struct remove_lvalue_reference<T>: std::type_identity<std::remove_reference_t<T>>{};

template<class T>
using remove_lvalue_reference_t = typename remove_lvalue_reference<T>::type;

template <class T>
struct remove_rvalue_reference: std::type_identity<T>{};

template <class T>
requires std::is_rvalue_reference_v<T>
struct remove_rvalue_reference<T>: std::type_identity<std::remove_reference_t<T>>{};

template <class T>
using remove_rvalue_reference_t = typename remove_rvalue_reference<T>::type;

template <class T>
struct requalify_as_const: std::conditional<
    std::is_lvalue_reference_v<T>,
    std::add_lvalue_reference_t<std::add_const_t<std::remove_reference<T>>>,
    std::conditional_t<std::is_rvalue_reference_v<T>,
    std::add_rvalue_reference_t<std::add_const_t<std::remove_reference_t<T>>>,
    std::add_const_t<T>>
>{};

template <class T>
using requalify_as_const_t = typename requalify_as_const<T>::type;

template <class T>
struct requalify_as_volatile;
template <class T>
using requalify_as_volatile_t = typename requalify_as_volatile<T>::type;

template <class T>
struct requalify_as_cv;
template <class T>
using requalify_as_cv_t = typename requalify_as_cv<T>::type;



template <class>
struct symbol_id {
    static constexpr auto singleton=[]{};
    static constexpr const void* address = std::addressof(singleton);
};
template <class Symbol, class T>
struct symbol_binder {
    using symbol_type = Symbol;
    using value_type = std::remove_cvref_t<T>;
    static constexpr Symbol symbol{};

    template <class U>
    requires std::is_convertible_v<U&&, requalify_as_const_t<remove_rvalue_reference_t<T>>>
    constexpr symbol_binder(Symbol, U&& x) noexcept(
        std::is_nothrow_convertible_v<U&&,requalify_as_const_t<remove_rvalue_reference_t<T>>>)
    : value(std::forward<U>(x)){}

    const value_type& operator()() const noexcept {return value;}
private:
    requalify_as_const_t<remove_rvalue_reference_t<T>> value;
};

template<class Symbol, class T>
symbol_binder(Symbol, T&&) -> symbol_binder<Symbol,T&&>;


struct unconstrained {
    template <class T>
    struct trait: std::true_type {};
};

struct real {
    template <class T>
    struct trait : std::is_floating_point<T>{};
};

template <class>
struct is_symbolic: std::false_type {};

template <class T>
inline constexpr bool is_symbolic_v = is_symbolic<T>::value;

template <class T>
concept symbolic = is_symbolic_v<T>;

template<typename B>
concept Binder = requires(const B b)
{
    B::symbol_type::id;
    // b->std::template is_convertible_v<B, requalify_as_const_t<remove_rvalue_reference_t<B>>>;
    // b ->template is_convertible_to<const B&>;
};




template <std::size_t I>
struct index_constant: std::integral_constant<std::size_t,I>{};

template <std::size_t I>
inline constexpr index_constant<I> index = {};

template <std::size_t I, Binder B>
struct substitution_element {
    using index = index_constant<I>;
    using id_type = decltype(B::symbol_type::id);
    constexpr substitution_element(const B& b): _binder(b){}
    constexpr const B& operator[](index) const {
        return _binder;
    }
    constexpr const B& operator[](id_type) const {
        return _binder;
    }

private: const B _binder;
};

template <class Sequence, class... T>
struct substitution_base;

template <class... Binders>
struct substitution;


template <class... Binders>
struct substitution: substitution_base<std::index_sequence_for<Binders...>, Binders...> {
    using base = substitution_base<std::index_sequence_for<Binders...>,Binders...>;
    using base::base;
    using base::operator[];
};

template <class... Binders>
substitution(const Binders&... )->substitution<Binders...>;

template <std::size_t... Index, class... Binders>
struct substitution_base<std::index_sequence<Index...>,Binders...>:
substitution_element<Index,Binders>...{
    using index_sequence = std::index_sequence<Index...>;
    using substitution_element<Index,Binders>::operator[]...;
    constexpr explicit substitution_base(const Binders&... x): substitution_element<Index,Binders>(x)...{}
};

template <class Trait = unconstrained, auto Id = symbol_id<decltype([]{})>{}>
struct symbol {
    static constexpr auto id = Id;
    template <class Arg>
    requires Trait::template trait<std::remove_cvref_t<Arg>>::value
    constexpr symbol_binder<symbol,Arg&&> operator = (Arg&& value) const{
        return symbol_binder(*this,std::forward<Arg>(value));
    }

    template <class... Binders>
    constexpr auto operator()(const substitution<Binders...> &s) const {
        return s[id]();
    }
};

template <class T, auto Id>
struct is_symbolic<symbol<T,Id>>: std::true_type{};

template <auto Value>
struct constant_symbol {
    using type = decltype(Value);
    static constexpr type value = Value;

    template <class... Binders>
    constexpr type operator()(const substitution<Binders...> &s) const {
        return value;
    }
};
typedef constant_symbol<0> Zero;
typedef constant_symbol<1> One;

template <auto Value>
struct is_symbolic<constant_symbol<Value>>: std::true_type {};

template <class Operator, symbolic... Terms>
struct symbolic_expression {
    template <class... Binders>
    constexpr auto operator()(const substitution<Binders...> &s) const noexcept {
        return Operator{}(Terms{}(s)...);
    }
};
template <class Operator, symbolic... Terms>
struct is_symbolic<symbolic_expression<Operator,Terms...>>: std::true_type {};

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

template<typename R, R value>
struct ToSymbolic:std::conditional_t<symbolic<R>,R,constant_symbol<value>> {};





template <typename  Op,typename  Lhs, typename  Rhs>
struct is_symbolic<BinaryOp<Op,Lhs,Rhs>>: std::true_type {};

struct plus_symbol {

    template <class Lhs, class Rhs>
    static constexpr auto apply(Lhs L, Rhs R){
        return L + R;
    }

    static constexpr auto apply(const std::valarray<double>& L, double R){
        return L+R;
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


    static constexpr auto apply(const std::valarray<double>& L, double R){
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

template <symbolic Expression>
struct formula {
    using expression = Expression;
    constexpr formula(Expression expr) noexcept {};
    template <class... Args>
    constexpr auto operator()(Args... args) const noexcept {
        return expression{}(substitution(args...));
    }

    // template <class... Args>
    // constexpr auto operator()(std::vector<double> values, Args... args) const noexcept {
    //     return expression{}(substitution(args...));
    // }
};


template <class Lhs,class Rhs>
constexpr bool operator<(symbol_id<Lhs>,symbol_id<Rhs>) {
    return std::less(symbol_id<Lhs>::address,symbol_id<Rhs>::address);
}

template< typename T>
struct add_const_ref :
std::conditional<std::is_reference_v<T>,T, T const &> {

};

#endif //T_TUT_H
