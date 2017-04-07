#include <type_traits>
namespace Carbon {
    namespace {
        template<typename F, typename T, std::size_t... I>
        auto ApplyImpl(F f, const T& t, std::index_sequence<I...>) -> decltype(f(std::get<I>(t)...)) {
            return f(std::get<I>(t)...);
        }
    }

    template<typename F, typename T>
    auto Apply(F f, const T& t) -> decltype(apply_impl(f, t, std::make_index_sequence<std::tuple_size<T>::value>())) {
        return ApplyImpl(f, t, std::make_index_sequence<std::tuple_size<T>::value>());
    }
}