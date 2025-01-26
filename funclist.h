#ifndef FUNCLIST_H
#define FUNCLIST_H

#include <functional>
#include <iostream>
#include <string>
#include <sstream>

namespace flist {

    constexpr auto empty = []([[maybe_unused]] auto f, auto a) {
        return a;
    };

    constexpr auto cons = [](auto x, auto l) {
        return [x, l](auto f, auto a) {
            return f(x, l(f, a));
        };
    };

    namespace detail {
        
        template <typename X, typename... Args>
        constexpr auto build_list(X&& x, Args&&... args) {
            if constexpr (sizeof...(args) == 0)
                return cons(std::forward<X>(x), empty);
            else
                return cons(std::forward<X>(x), build_list(std::forward<Args>(args)...));
        }

        constexpr auto foldl = [](auto l, auto f, auto a) -> decltype(auto) {
            return l([&](auto x, auto acc) -> decltype(auto) {
                return f(acc, x);
            }, a);
        };

        constexpr auto foldr = [](auto l, auto f, auto a) -> decltype(auto) {
            return l([&](auto x, auto acc) -> decltype(auto) {
                return f(x, acc);
            }, a);
        };

        constexpr auto r_to_vector(auto r) {
            using value_t = std::decay_t<decltype(*r.begin())>;
            std::vector<value_t> v(r.begin(), r.end());
            return v;
        };

        template <typename R>
        constexpr auto r_to_vector(std::reference_wrapper<R> r) {
            return r_to_vector(r.get());
        }
    }

    constexpr auto create = [](auto... args) {
        return detail::build_list(args...);
    };

    constexpr auto of_range = [](auto r) {
        auto v = detail::r_to_vector(r);

        return [v = std::move(v)](auto f, auto acc) {
            for(auto it = v.rbegin(); it != v.rend(); ++it)
                acc = f(*it, acc);
            return acc;
        };
    };

    constexpr auto concat = [](auto l, auto k) {
        return [=](auto f, auto acc) {
            auto tmp = k(f, acc);
            return l(f, tmp);
        };
    };
    
    constexpr auto rev = [](auto l) {
        return [=](auto f, auto a) {
            using A = decltype(a);
            std::function<A(A)> acc =
                l(
                    [=](auto x, std::function<A(A)> old_acc) -> std::function<A(A)> {
                        return [=](A current) -> A {
                            return old_acc(f(x, current));
                        };
                    },
                    std::function<A(A)>([](A init) { return init; })
                );
            return acc(a);
        };
    };

    constexpr auto map = [](auto m, auto l) {
        return [=](auto f, auto acc) {
            return l([&](auto x, auto acc_next) {
                return f(m(x), acc_next);
            }, acc);
        };
    };

    constexpr auto filter = [](auto p, auto l) {
        return [=](auto f, auto acc) {
            return l([&](auto x, auto acc_next) {
                if (p(x))
                    return f(x, acc_next);
                return acc_next;
            }, acc);
        };
    };

    constexpr auto flatten = [](auto l) {
        return [=](auto f, auto acc) {
            return l(
                [&](auto sublist, auto currentAcc) {
                    return sublist(f, currentAcc);
                },
                acc
            );
        };
    };

    constexpr auto as_string = [](const auto& l) -> std::string {
        auto fold_function = [&](const auto& x, const std::string& acc) -> std::string {
            std::stringstream ss;
            ss << x;
            std::string x_str = ss.str();

            if (acc.empty()) {
                return x_str;
            } else {
                return x_str + ";" + acc;
            }
        };

        std::string result = detail::foldr(l, fold_function, std::string(""));

        return "[" + result + "]";
    };

};

#endif // FUNCLIST_H