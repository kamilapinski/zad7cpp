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
            return l(f, f(x, a));
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

    }

    // funkcja zwracająca listę składającą się z podanych argumentów
    constexpr auto create = [](auto... args) {
        return detail::build_list(args...);
    };

    // funkcja zwracająca listę powstałą z elementów r; można założyć, że r jest typu spełniającego koncept std::ranges::bidirectional_range ewentualnie opakowanego w std::reference_wrapper
    constexpr auto of_range = [](auto r) {
        // TODO: implement
        (void)r;
    };

    // funkcja zwracająca listę powstałą z połączenia list l i k
    constexpr auto concat = [](auto l, auto k) {
        // TODO: implement
        (void)l;
        (void)k;
    };
    
    // funkcja zwracająca listę z odwróconą kolejnością elementów listy l
    constexpr auto rev = [](auto l) {
        return [l](auto f, auto a) {
            using A = decltype(a);

            std::function<A(A)> acc = l(
                [&](auto x, std::function<A(A)> prev) {
                    return [=, &f](A a0) {
                        return prev(f(x, a0));
                    };
                },
                std::function<A(A)>([](A a0){ return a0; })
            );

            return acc(a);
        };
    };

    // funkcja zwracająca listę powstałą z listy l w taki sposób, że każdy jej element x zamieniany jest na m(x)
    constexpr auto map = [](auto m, auto l) {
        // TODO: implement
        (void)m;
        (void)l;
    };

    // funkcja zwracająca listę powstałą z listy l poprzez zostawienie tylko takich elementów x, które spełniają predykat p(x)
    constexpr auto filter = [](auto p, auto l) {
        // TODO: implement
        (void)p;
        (void)l;
    };

    // funkcja zwracająca listę powstałą z połączenia list pamiętanych w liście list l
    constexpr auto flatten = [](auto l) {
        // TODO: implement
        (void)l;
    };
    
    // funkcja zwracająca reprezentację listy l jako std::string przy założeniu, że dla każdego elementu listy x działa os << x, gdzie os jest obiektem pochodnym basic_ostream; patrz przykłady użycia
    constexpr auto as_string = [](const auto& l) -> std::string {
        std::stringstream ss;
        ss << '[';

        l([&ss](const auto& x, const auto& next) {
            ss << x << ';';
            return next;
        }, empty);

        auto str = ss.str();
        if (str.size() > 1)
            str.pop_back();
        
        return str + ']';

        return str;
    };

};

#endif // FUNCLIST_H