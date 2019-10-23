#include "lambda.hpp"
#include <iostream>

namespace util
{
template <std::size_t N>
using index_constant = std::integral_constant<std::size_t, N>;
}

template <std::size_t N>
void testcase(util::index_constant<N>);

template <std::size_t... I>
void test(std::index_sequence<I...>)
{
    ((testcase(util::index_constant<I>()), std::cout << "------" << std::endl), ...);
}

int main()
{
    test(std::index_sequence<0, 1, 2, 3, 4, 5, 6, 7, 8>());
    return 0;
}

// below are testcases
template <>
void testcase(util::index_constant<0>)
{
    Lambda::LambdaPtr expr = nullptr;
    {
        auto x = Lambda::make_id("x");
        auto y = Lambda::make_id("y");
        auto l = Lambda::make_abstr(x, Lambda::make_app(x, y));
        expr = Lambda::make_app(l, y);
    }
    std::cout << Lambda::to_string(*expr) << std::endl;
    expr = Lambda::beta_reduction(expr);
    std::cout << "    ->* " << Lambda::to_string(*expr) << std::endl;
}

template <>
void testcase(util::index_constant<1>)
{
    Lambda::LambdaPtr expr = nullptr;
    {
        auto x = Lambda::make_id("x");
        auto y = Lambda::make_id("y");
        auto l = Lambda::make_abstr(x, Lambda::make_abstr(y, Lambda::make_app(x, y)));
        expr = Lambda::make_app(l, Lambda::make_id("y"));
    }
    std::cout << Lambda::to_string(*expr) << std::endl;
    expr = Lambda::beta_reduction(expr);
    std::cout << "    ->* " << Lambda::to_string(*expr) << std::endl;
}

template <>
void testcase(util::index_constant<2>)
{
    Lambda::LambdaPtr expr = nullptr;
    {
        auto x0 = Lambda::make_id("x");
        auto x1 = Lambda::make_id("x");
        auto y = Lambda::make_id("y");
        auto l0 = Lambda::make_abstr(x0, Lambda::make_app(x0, Lambda::make_abstr(x1, Lambda::make_app(x1, y))));
        expr = Lambda::make_app(l0, y);
    }
    std::cout << Lambda::to_string(*expr) << std::endl;
    expr = Lambda::beta_reduction(expr);
    std::cout << "    ->* " << Lambda::to_string(*expr) << std::endl;
}

template <>
void testcase(util::index_constant<3>)
{
    Lambda::LambdaPtr expr = nullptr;
    {
        auto f = Lambda::make_id("f");
        auto x0 = Lambda::make_id("x");
        auto x1 = Lambda::make_id("x");
        auto twice = Lambda::make_abstr(f, Lambda::make_abstr(x0, Lambda::make_app(f, Lambda::make_app(f, x0))));
        auto id = Lambda::make_abstr(x1, x1);
        expr = Lambda::make_app(twice, id);
    }
    std::cout << Lambda::to_string(*expr) << std::endl;
    expr = Lambda::beta_reduction(expr);
    std::cout << "    ->* " << Lambda::to_string(*expr) << std::endl;
}

template <>
void testcase(util::index_constant<4>)
{
    Lambda::LambdaPtr expr = nullptr;
    {
        auto f = Lambda::make_id("f");
        auto x0 = Lambda::make_id("x");
        auto x1 = Lambda::make_id("x");
        auto y = Lambda::make_id("y");
        auto l = Lambda::make_abstr(f, Lambda::make_app(Lambda::make_app(f, y), x0));
        auto r = Lambda::make_abstr(x1, Lambda::make_abstr(Lambda::make_id("y"), x1));
        expr = Lambda::make_app(l, r);
    }
    std::cout << Lambda::to_string(*expr) << std::endl;
    expr = Lambda::beta_reduction(expr);
    std::cout << "    ->* " << Lambda::to_string(*expr) << std::endl;
}

template <>
void testcase(util::index_constant<5>)
{
    Lambda::LambdaPtr expr = nullptr;
    {
        auto x0 = Lambda::make_id("x");
        auto x1 = Lambda::make_id("x");
        auto tw0 = Lambda::make_abstr(x0, Lambda::make_app(x0, x0));
        auto tw1 = Lambda::make_abstr(x1, Lambda::make_app(x1, x1));
        auto y = Lambda::make_abstr(Lambda::make_id("x"), Lambda::make_id("y"));
        expr = Lambda::make_app(y, Lambda::make_app(tw0, tw1));
    }
    std::cout << Lambda::to_string(*expr) << std::endl;
    expr = Lambda::beta_reduction(expr);
    std::cout << "    ->* " << Lambda::to_string(*expr) << std::endl;
}

template <>
void testcase(util::index_constant<6>)
{
    Lambda::LambdaPtr expr = nullptr;
    {
        auto x = Lambda::make_id("x");
        auto y = Lambda::make_id("y");
        auto z = Lambda::make_id("z");
        auto l = Lambda::make_abstr(x, Lambda::make_abstr(y, Lambda::make_app(x, y)));
        auto r = Lambda::make_abstr(z, Lambda::make_app(z, Lambda::make_id("y")));
        expr = Lambda::make_app(l, r);
    }
    std::cout << Lambda::to_string(*expr) << std::endl;
    expr = Lambda::beta_reduction(expr);
    std::cout << "    ->* " << Lambda::to_string(*expr) << std::endl;
}

template <>
void testcase(util::index_constant<7>)
{
    Lambda::LambdaPtr expr = nullptr;
    {
        auto f = Lambda::make_id("f");
        auto l = Lambda::make_abstr(f,
            Lambda::make_app(
                Lambda::make_app(f, Lambda::make_id("x")),
                Lambda::make_app(f, Lambda::make_id("y"))));
        auto x = Lambda::make_id("x");
        auto y = Lambda::make_id("y");
        auto l1 = Lambda::make_abstr(x, Lambda::make_abstr(y, y));
        auto r = Lambda::make_app(l1, Lambda::make_id("w"));
        expr = Lambda::make_app(l, r);
    }
    std::cout << Lambda::to_string(*expr) << std::endl;
    expr = Lambda::beta_reduction(expr);
    std::cout << "    ->* " << Lambda::to_string(*expr) << std::endl;
}

template <>
void testcase(util::index_constant<8>)
{
    Lambda::LambdaPtr expr = nullptr;
    {
        auto f = Lambda::make_id("f");
        auto x0 = Lambda::make_id("x");
        auto x1 = Lambda::make_id("x");
        auto d0 = Lambda::make_abstr(x0, Lambda::make_app(x0, x0));
        auto d1 = Lambda::make_abstr(x1, Lambda::make_app(x1, x1));
        auto omega = Lambda::make_app(d0, d1);
        auto l = Lambda::make_abstr(f, Lambda::make_app(f, omega));
        auto r = Lambda::make_abstr(Lambda::make_id("x"), Lambda::make_id("y"));
        expr = Lambda::make_app(l, r);
    }
    std::cout << Lambda::to_string(*expr) << std::endl;
    expr = Lambda::beta_reduction(expr);
    std::cout << "    ->* " << Lambda::to_string(*expr) << std::endl;
}
