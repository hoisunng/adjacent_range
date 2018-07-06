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

template <std::size_t N, typename FwdIt>
class IterTuple
{
    using Ref = typename std::iterator_traits<FwdIt>::reference;

  public:
    using type = tuple_of<N, FwdIt>;
    using value_type = tuple_of<N, Ref>;
    using underlying_type = FwdIt;
    static constexpr auto size = N;
};

template <typename FwdIt>
class IterTuple<2, FwdIt>
{
    using Ref = typename std::iterator_traits<FwdIt>::reference;

  public:
    using type = std::pair<FwdIt, FwdIt>;
    using value_type = std::pair<Ref, Ref>;
    using underlying_type = FwdIt;
    static constexpr auto size = 2;
};

template <typename FwdIt>
FwdIt init(FwdIt first, FwdIt last, std::size_t n)
{
    const auto diff = std::distance(first, last);
    return diff <= n ? last : std::next(first, n);
}

template <typename IterTuple, typename FwdIt, std::size_t... Is>
typename IterTuple::type init(FwdIt first, FwdIt last, std::index_sequence<Is...>)
{
    return typename IterTuple::type{init(first, last, Is)...};
}

template <typename IterTuple, std::size_t... Is>
IterTuple increment(IterTuple iter, std::index_sequence<Is...>)
{
    return IterTuple{std::next(std::get<Is>(iter))...};
}

template <typename IterTuple, std::size_t... Is>
typename IterTuple::value_type dereference(typename IterTuple::type iter, std::index_sequence<Is...>)
{
    return typename IterTuple::value_type{(*std::get<Is>(iter))...};
}

template <typename IterTuple>
typename IterTuple::underlying_type last(typename IterTuple::type iter)
{
    return std::get<IterTuple::size - 1>(iter);
}

} // namespace detail

template <typename IterTuple>
class adjacent_iterator
{
    static constexpr auto N = IterTuple::size;
    using FwdIt = typename IterTuple::underlying_type;

  public:
    using difference_type = std::ptrdiff_t;
    using value_type = typename IterTuple::value_type;
    using pointer = value_type *;
    using reference = value_type;
    using iterator_category = std::forward_iterator_tag;

    adjacent_iterator(FwdIt first, FwdIt last)
        : m_iter{detail::init<IterTuple>(first, last, std::make_index_sequence<N>{})}
    {
    }

    bool operator==(const adjacent_iterator &other) const
    {
        return detail::last<IterTuple>(m_iter) == detail::last<IterTuple>(other.m_iter);
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
        return detail::dereference<IterTuple>(m_iter, std::make_index_sequence<N>{});
    }

  private:
    typename IterTuple::type m_iter;
};

template <std::size_t N, typename FwdIt>
class adjacent_range
{
    using IterTuple = detail::IterTuple<N, FwdIt>;

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