#include <iterator>
#include <utility>
#include <tuple>

namespace hsng
{
namespace detail
{
template <std::size_t I, typename T>
struct tuple_n
{
    template <typename... Args>
    using type = typename tuple_n<I - 1, T>::template type<T, Args...>;
};

template <typename T>
struct tuple_n<0, T>
{
    template <typename... Args>
    using type = std::tuple<Args...>;
};
template <std::size_t I, typename T>
using tuple_of = typename tuple_n<I, T>::template type<>;

template <typename FwdIt>
FwdIt init(FwdIt first, FwdIt last, std::size_t n)
{
    const auto diff = std::distance(first, last);
    return diff <= n ? last : std::next(first, n);
}

template <typename FwdIt, std::size_t... Is>
tuple_of<sizeof...(Is), FwdIt> init(FwdIt first, FwdIt last, std::index_sequence<Is...>)
{
    return tuple_of<sizeof...(Is), FwdIt>{init(first, last, Is)...};
}

template <typename... FwdIts, std::size_t... Is>
std::tuple<FwdIts...> increment(std::tuple<FwdIts...> iter, std::index_sequence<Is...>)
{
    return std::tuple<FwdIts...>{std::next(std::get<Is>(iter))...};
}

template <typename... FwdIts, std::size_t... Is>
std::tuple<typename std::iterator_traits<FwdIts>::reference...> dereference(std::tuple<FwdIts...> iter, std::index_sequence<Is...>)
{
    return std::tuple<typename std::iterator_traits<FwdIts>::reference...>{(*std::get<Is>(iter))...};
}
} // namespace detail

template <typename IterTuple>
class adjacent_iterator
{
    static constexpr auto N = std::tuple_size<IterTuple>::value;
    using FwdIt = typename std::tuple_element<0, IterTuple>::type;

  public:
    using difference_type = std::ptrdiff_t;
    using value_type = detail::tuple_of<N, typename std::iterator_traits<FwdIt>::reference>;
    using pointer = value_type *;
    using reference = value_type;
    using iterator_category = std::forward_iterator_tag;

    adjacent_iterator(FwdIt first, FwdIt last)
        : m_iter{detail::init(first, last, std::make_index_sequence<N>{})}
    {
    }

    bool operator==(const adjacent_iterator &other) const
    {
        return std::get<N - 1>(m_iter) == std::get<N - 1>(other.m_iter);
    }
    bool operator!=(const adjacent_iterator &other) const
    {
        return !operator==(other);
    }

    adjacent_iterator &operator++()
    {
        m_iter = detail::increment(m_iter, std::make_index_sequence<N>{});
        return *this;
    }
    adjacent_iterator operator++(int)
    {
        m_iter = detail::increment(m_iter, std::make_index_sequence<N>{});
        return *this;
    }

    value_type operator*() const
    {
        return detail::dereference(m_iter, std::make_index_sequence<N>{});
    }

  private:
    IterTuple m_iter;
};

template <std::size_t N, typename FwdIt>
class adjacent_range
{
    using IterTuple = detail::tuple_of<N, FwdIt>;

  public:
    using iterator = adjacent_iterator<IterTuple>;
    using const_iterator = adjacent_iterator<IterTuple>;

    adjacent_range(FwdIt first, FwdIt last)
        : m_first{first}, m_last{last} {}

    adjacent_iterator<IterTuple> begin() const
    {
        return adjacent_iterator<IterTuple>{m_first, m_last};
    }

    adjacent_iterator<IterTuple> end() const
    {
        return adjacent_iterator<IterTuple>{m_last, m_last};
    }

  private:
    FwdIt m_first;
    FwdIt m_last;
};

template <std::size_t N, typename C>
auto make_adjacent_range(C &c) -> adjacent_range<N, decltype(c.begin())>
{
    return adjacent_range<N, decltype(c.begin())>{c.begin(), c.end()};
}

} // namespace hsng