#include <iostream>
#include <functional>
using namespace std;

template <typename T>
struct function_traits;

template <typename R, typename... Args>
struct function_traits<R(Args...)> {
	using ret_type = R;
	using stl_func_type = std::function<R(Args...)>;
	static constexpr std::size_t arity = sizeof...(Args);

	template <std::size_t N>
	struct argument {
		static_assert(N < arity, "error, invalid parameter index");
		using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
	};
};

template <typename R, typename C, typename... Args>
struct function_traits<R(C&, Args...)> :public function_traits <R(Args...)> {};

template <typename R,typename... Args>
struct function_traits<R(*)(Args...)> : public function_traits<R(Args...)>{};

//member function pointer
template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...)> :public function_traits<R(C&,Args...)> {};

//const member function pointer
template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...) const> :public function_traits<R(C&, Args...)> {};

template <typename C, typename R>
struct function_traits<R(C::*)> :public function_traits<R(C&)> {};

template <typename F>
struct function_traits {
private:
	using call_type = function_traits<decltype(&F::operator())>;
public:
	using ret_type = typename call_type::ret_type;
	using stl_func_type = typename call_type::stl_func_type;
	static constexpr std::size_t arity = call_type::arity - 1;

	template <std::size_t N>
	struct argument {
		static_assert(N < arity, "error: invalid parameter index");
		using type = typename call_type::template argument<N + 1>::type;
	};
};

template <typename F>
struct function_traits<F&>: public function_traits<F>{};

template <typename F>
struct function_traits<F&&> : public function_traits<F> {};


int f(int a, int b) {
	return a + b;
}

class A {
public:
	int operator() (int x) {
		return x;
	}
};

auto z = [](int x) {return x; };

int main() {
	A a;
	function<int(int)> sa = static_cast<function<int(int)>>(a);
	using Traits = function_traits<int(int,int)>;
	function<int(int, int)> sf(f);
	static_assert(std::is_same<decltype(sf),function_traits<decltype(f)>::stl_func_type>::value,"");
	static_assert(Traits::arity==2,"");
	static_assert(std::is_same<Traits::ret_type,int>::value,"");
	static_assert(std::is_same<Traits::argument<1>::type, int>::value, "");
	static_assert(std::is_same<function_traits<A>::ret_type,int>::value,"");
	static_assert(std::is_same < function_traits<decltype(z)> ::ret_type, int > ::value, "");
	static_assert(std::is_same<decltype(sa), function_traits<decltype(a)>::stl_func_type>::value, "");
	system("pause");
}