#ifndef FUNCLIST_H
#define FUNCLIST_H

#include <functional>
#include <iostream>
#include <string>
#include <sstream>

namespace flist {

    // funkcja stała reprezentująca listę pustą
    constexpr auto empty = []([[maybe_unused]] auto f, auto a) {
        return a;
    };

    // funkcja zwracająca listę l z dodanym na jej początek x
    constexpr auto cons = [](auto x, auto l) {
        return [x, l](auto f, auto a) {
            //return l(f, f(x, a));
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
    }

    // funkcja zwracająca listę składającą się z podanych argumentów
    constexpr auto create = [](auto... args) {
        return detail::build_list(args...);
    };

    // funkcja zwracająca listę powstałą z elementów r; można założyć, że r jest typu spełniającego koncept std::ranges::bidirectional_range ewentualnie opakowanego w std::reference_wrapper
    constexpr auto of_range = [](auto r) {
        using value_t = std::decay_t<decltype(*r.begin())>;
        std::vector<value_t> v(r.begin(), r.end());
        std::reverse(v.begin(), v.end());

        return [v = std::move(v)](auto f, auto acc) {
            for (auto const& elt : v)
                acc = f(elt, acc);
            return acc;
        };
    };

    // funkcja zwracająca listę powstałą z połączenia list l i k
    constexpr auto concat = [](auto l, auto k) {
        return [=](auto f, auto acc) {
            auto tmp = k(f, acc);
            return l(f, tmp);
        };
    };
    
    // funkcja zwracająca listę z odwróconą kolejnością elementów listy l
    // Remove std::function from rev:
    constexpr auto rev = [](auto l) {
        return [=](auto f, auto a) {
            // A is the type of 'a'
            using A = decltype(a);

            // Fold over the list l, building up std::function<A(A)>
            std::function<A(A)> agg =
                l(
                    // For each element x, update the accumulator
                    [=](auto x, std::function<A(A)> oldAgg) -> std::function<A(A)> {
                        // Return a new function from A -> A
                        return [=](A current) -> A {
                            // Apply f(x, ...) then feed into oldAgg
                            return oldAgg(f(x, current));
                        };
                    },
                    // Start with identity
                    std::function<A(A)>([](A init) { return init; })
                );

            // Finally, apply that function to 'a'
            return agg(a);
        };
    };

    // funkcja zwracająca listę powstałą z listy l w taki sposób, że każdy jej element x zamieniany jest na m(x)
    constexpr auto map = [](auto m, auto l) {
        return [=](auto f, auto acc) {
            return l([&](auto x, auto acc_next) {
                return f(m(x), acc_next);
            }, acc);
        };
    };

    // funkcja zwracająca listę powstałą z listy l poprzez zostawienie tylko takich elementów x, które spełniają predykat p(x)
    constexpr auto filter = [](auto p, auto l) {
        return [=](auto f, auto acc) {
            return l([&](auto x, auto acc_next) {
                if (p(x))
                    return f(x, acc_next);
                return acc_next;
            }, acc);
        };
    };

    // funkcja zwracająca listę powstałą z połączenia list pamiętanych w liście list l
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
    
    // funkcja zwracająca reprezentację listy l jako std::string przy założeniu, że dla każdego elementu listy x działa os << x, gdzie os jest obiektem pochodnym basic_ostream; patrz przykłady użycia

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

        // Perform foldr to accumulate the string
        std::string result = detail::foldr(l, fold_function, std::string(""));

        // Enclose the result within square brackets
        return "[" + result + "]";
    };

};

#endif // FUNCLIST_H