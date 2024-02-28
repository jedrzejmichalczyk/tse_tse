

#ifndef SYMBOL_H
#define SYMBOL_H
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
#endif //SYMBOL_H