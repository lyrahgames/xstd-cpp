// Copyright © 2026 Markus Pawellek
//
// This file is part of `xstd`.
//
// `xstd` is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.
//
// `xstd` is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with `xstd`. If not, see <https://www.gnu.org/licenses/>.
//
#undef NDEBUG
#include <cassert>
import std;
import xstd;

// Basic types are not tuple-like.
static_assert(not xstd::tuple_like<int>);
static_assert(not xstd::tuple_like<int&>);
static_assert(not xstd::tuple_like<int const&>);
static_assert(not xstd::tuple_like<float>);
static_assert(not xstd::tuple_like<std::string>);
static_assert(not xstd::tuple_like<std::thread>);
static_assert(not xstd::tuple_like<std::vector<int>>);
static_assert(not xstd::tuple_like<std::unique_ptr<int>>);

// `std::pair` instances are tuple-like.
static_assert(xstd::tuple_like<std::pair<int, float>>);
static_assert(xstd::tuple_like<std::pair<int, float>&>);
static_assert(xstd::tuple_like<std::pair<int, float>&&>);
static_assert(xstd::tuple_like<std::pair<int, float> const>);
static_assert(xstd::tuple_like<std::pair<int, float> const&>);
static_assert(xstd::tuple_like<std::pair<int, float> const&&>);

// `std::array` instances are tuple-like.
static_assert(xstd::tuple_like<std::array<int, 3>>);
static_assert(xstd::tuple_like<std::array<int, 3>&>);
static_assert(xstd::tuple_like<std::array<int, 3>&&>);
static_assert(xstd::tuple_like<std::array<int, 3> const>);
static_assert(xstd::tuple_like<std::array<int, 3> const&>);
static_assert(xstd::tuple_like<std::array<int, 3> const&&>);
static_assert(xstd::tuple_like<std::array<const int, 3>>);
static_assert(xstd::tuple_like<std::array<const int, 3>&>);
static_assert(xstd::tuple_like<std::array<const int, 3>&&>);
static_assert(xstd::tuple_like<std::array<const int, 3> const>);
static_assert(xstd::tuple_like<std::array<const int, 3> const&>);
static_assert(xstd::tuple_like<std::array<const int, 3> const&&>);
static_assert(xstd::tuple_like<std::array<std::unique_ptr<int>, 5>>);
static_assert(xstd::tuple_like<std::array<float, 0>>);
static_assert(xstd::tuple_like<std::array<std::string, 5>>);

// `std::tuple` instances are tuple-like.
static_assert(xstd::tuple_like<std::tuple<>>);
static_assert(xstd::tuple_like<std::tuple<int>>);
static_assert(xstd::tuple_like<std::tuple<int, float>>);
static_assert(xstd::tuple_like<std::tuple<int, float, std::string>>);
static_assert(xstd::tuple_like<std::tuple<int, float, std::string>&>);
static_assert(xstd::tuple_like<std::tuple<int, float, std::string>&&>);
static_assert(xstd::tuple_like<std::tuple<int, float, std::string> const>);
static_assert(xstd::tuple_like<std::tuple<int, float, std::string> const&>);
static_assert(xstd::tuple_like<std::tuple<int, float, std::string> const&&>);
static_assert(xstd::tuple_like<std::tuple<int, float, std::unique_ptr<int>>>);
static_assert(xstd::tuple_like<std::tuple<int, float, std::unique_ptr<int>>&>);
static_assert(xstd::tuple_like<std::tuple<int, float, std::unique_ptr<int>>&&>);
static_assert(xstd::tuple_like<std::tuple<int, std::unique_ptr<int>> const>);
static_assert(xstd::tuple_like<std::tuple<int, std::unique_ptr<int>> const&>);
static_assert(xstd::tuple_like<std::tuple<int, std::unique_ptr<int>> const&&>);
static_assert(xstd::tuple_like<
              std::tuple<int, float, std::string, std::unique_ptr<int>>>);

static_assert(xstd::strict_tuple_like<std::tuple<>>);
static_assert(xstd::strict_tuple_like<std::tuple<int>>);
static_assert(xstd::strict_tuple_like<std::tuple<int, float>>);
static_assert(xstd::strict_tuple_like<std::tuple<int, float, std::string>>);
static_assert(xstd::strict_tuple_like<std::tuple<int, float, std::string>&>);
static_assert(xstd::strict_tuple_like<std::tuple<int, float, std::string>&&>);
static_assert(
    xstd::strict_tuple_like<std::tuple<int, float, std::string> const>);
static_assert(
    xstd::strict_tuple_like<std::tuple<int, float, std::string> const&>);
static_assert(
    xstd::strict_tuple_like<std::tuple<int, float, std::string> const&&>);
static_assert(
    xstd::strict_tuple_like<std::tuple<int, float, std::unique_ptr<int>>>);
static_assert(
    xstd::strict_tuple_like<std::tuple<int, float, std::unique_ptr<int>>&>);
static_assert(
    xstd::strict_tuple_like<std::tuple<int, float, std::unique_ptr<int>>&&>);
static_assert(
    xstd::strict_tuple_like<std::tuple<int, std::unique_ptr<int>> const>);
static_assert(
    xstd::strict_tuple_like<std::tuple<int, std::unique_ptr<int>> const&>);
static_assert(
    xstd::strict_tuple_like<std::tuple<int, std::unique_ptr<int>> const&&>);
static_assert(xstd::strict_tuple_like<
              std::tuple<int, float, std::string, std::unique_ptr<int>>>);

// Custom tuple-like types can provide a `get<index>()` member function template.
struct member_gettable_tuple {
  int x;
  float y;
  template <std::size_t index>
  constexpr decltype(auto) get(this auto&& self) {
    if constexpr (index == 0)
      return std::forward_like<decltype(self)>(self.x);
    else
      return std::forward_like<decltype(self)>(self.y);
  }
};
namespace std {
template <>
struct tuple_size<member_gettable_tuple>
    : std::integral_constant<std::size_t, 2> {};
template <>
struct tuple_element<0, member_gettable_tuple> {
  using type = int;
};
template <>
struct tuple_element<1, member_gettable_tuple> {
  using type = float;
};
}  // namespace std

static_assert(xstd::strict_tuple_like<member_gettable_tuple>);
static_assert(xstd::strict_tuple_like<member_gettable_tuple>);
static_assert(xstd::strict_tuple_like<member_gettable_tuple&>);
static_assert(xstd::strict_tuple_like<member_gettable_tuple&&>);
static_assert(xstd::strict_tuple_like<member_gettable_tuple const>);
static_assert(xstd::strict_tuple_like<member_gettable_tuple const&>);
static_assert(xstd::strict_tuple_like<member_gettable_tuple const&&>);

static_assert(xstd::tuple_like<member_gettable_tuple>);
static_assert(xstd::tuple_like<member_gettable_tuple&>);
static_assert(xstd::tuple_like<member_gettable_tuple&&>);
static_assert(xstd::tuple_like<member_gettable_tuple const>);
static_assert(xstd::tuple_like<member_gettable_tuple const&>);
static_assert(xstd::tuple_like<member_gettable_tuple const&&>);

// Custom tuple-like types may provide ADL-enabled `get<index>(tuple)` function.
namespace adl_space {
struct adl_gettable_tuple {
  int a;
  int b;
};
template <typename type>
concept adl_tuple_like =
    std::same_as<adl_gettable_tuple, std::remove_cvref_t<type>>;
template <std::size_t index>
constexpr auto&& get(adl_tuple_like auto&& tuple) {
  if constexpr (index == 0)
    return std::forward<decltype(tuple)>(tuple).a;
  else
    return std::forward<decltype(tuple)>(tuple).b;
}
}  // namespace adl_space
namespace std {
template <>
struct tuple_size<adl_space::adl_gettable_tuple>
    : std::integral_constant<std::size_t, 2> {};
template <std::size_t I>
struct tuple_element<I, adl_space::adl_gettable_tuple> {
  using type = int;
};
}  // namespace std

static_assert(xstd::tuple_like<adl_space::adl_gettable_tuple>);
static_assert(xstd::tuple_like<adl_space::adl_gettable_tuple&>);
static_assert(xstd::tuple_like<adl_space::adl_gettable_tuple&&>);
static_assert(xstd::tuple_like<adl_space::adl_gettable_tuple const>);
static_assert(xstd::tuple_like<adl_space::adl_gettable_tuple const&>);
static_assert(xstd::tuple_like<adl_space::adl_gettable_tuple const&&>);

static_assert(xstd::strict_tuple_like<adl_space::adl_gettable_tuple>);
static_assert(xstd::strict_tuple_like<adl_space::adl_gettable_tuple&>);
static_assert(xstd::strict_tuple_like<adl_space::adl_gettable_tuple&&>);
static_assert(xstd::strict_tuple_like<adl_space::adl_gettable_tuple const>);
static_assert(xstd::strict_tuple_like<adl_space::adl_gettable_tuple const&>);
static_assert(xstd::strict_tuple_like<adl_space::adl_gettable_tuple const&&>);

// Custom tuple-like types that define both member and ADL-based
// `get` function templates prioritize the member function.
// `get` functions that return prvalues, ie. temporaries, for in-place modification
// can still be considered tuple-like. This behavior can be counter-intuitive in
// some situations because such types do not provide references to change their
// stored data. Such types are not considered to be strictly tuple-like.
namespace adl_space {
struct member_and_adl_gettable_tuple {
  int data;
  template <std::size_t index>
  constexpr auto get() const {
    return data + 1;
  }
};
template <std::size_t index>
constexpr auto get(member_and_adl_gettable_tuple const& tuple) {
  return tuple.data;
}
}  // namespace adl_space
namespace std {
template <>
struct tuple_size<adl_space::member_and_adl_gettable_tuple>
    : std::integral_constant<std::size_t, 1> {};
template <std::size_t index>
struct tuple_element<index, adl_space::member_and_adl_gettable_tuple> {
  using type = int;
};
}  // namespace std

static_assert(
    not xstd::strict_tuple_like<adl_space::member_and_adl_gettable_tuple>);
static_assert(
    not xstd::strict_tuple_like<adl_space::member_and_adl_gettable_tuple&>);
static_assert(
    not xstd::strict_tuple_like<adl_space::member_and_adl_gettable_tuple&&>);
static_assert(not xstd::strict_tuple_like<
              adl_space::member_and_adl_gettable_tuple const>);
static_assert(not xstd::strict_tuple_like<
              adl_space::member_and_adl_gettable_tuple const&>);
static_assert(not xstd::strict_tuple_like<
              adl_space::member_and_adl_gettable_tuple const&&>);

static_assert(xstd::tuple_like<adl_space::member_and_adl_gettable_tuple>);
static_assert(xstd::tuple_like<adl_space::member_and_adl_gettable_tuple&>);
static_assert(xstd::tuple_like<adl_space::member_and_adl_gettable_tuple&&>);
static_assert(xstd::tuple_like<adl_space::member_and_adl_gettable_tuple const>);
static_assert(
    xstd::tuple_like<adl_space::member_and_adl_gettable_tuple const&>);
static_assert(
    xstd::tuple_like<adl_space::member_and_adl_gettable_tuple const&&>);

// Types that do not provide any `get` function are not considered tuple-like.
struct not_gettable_tuple {
  int x;
};
namespace std {
template <>
struct tuple_size<not_gettable_tuple> : std::integral_constant<std::size_t, 1> {
};
template <>
struct tuple_element<0, not_gettable_tuple> {
  using type = int;
};
}  // namespace std

static_assert(not xstd::tuple_like<not_gettable_tuple>);
static_assert(not xstd::tuple_like<not_gettable_tuple&>);
static_assert(not xstd::tuple_like<not_gettable_tuple&&>);
static_assert(not xstd::tuple_like<not_gettable_tuple const>);
static_assert(not xstd::tuple_like<not_gettable_tuple const&>);
static_assert(not xstd::tuple_like<not_gettable_tuple const&&>);

// Types whose `get` return types are not convertible to their
// prescribed element's type are not considered as tuple-likes.
struct bad_return_tuple {
  int x;
};
namespace std {
template <>
struct tuple_size<bad_return_tuple> : std::integral_constant<std::size_t, 1> {};
template <>
struct tuple_element<0, bad_return_tuple> {
  using type = int;
};
}  // namespace std
template <std::size_t I>
auto get(bad_return_tuple) -> std::string {
  return "oops";
}

static_assert(not xstd::tuple_like<bad_return_tuple>);
static_assert(not xstd::tuple_like<bad_return_tuple&>);
static_assert(not xstd::tuple_like<bad_return_tuple&&>);
static_assert(not xstd::tuple_like<bad_return_tuple const>);
static_assert(not xstd::tuple_like<bad_return_tuple const&>);
static_assert(not xstd::tuple_like<bad_return_tuple const&&>);

// Custom types that define a free `get` function that is
// not reachable via ADL are not considered to be tuple-like.
namespace adl_space {
struct non_adl_tuple {
  int a, b;
};
template <typename type>
concept non_adl_tuple_like =
    std::same_as<non_adl_tuple, std::remove_cvref_t<type>>;
}  // namespace adl_space

template <std::size_t index>
constexpr auto&& get(adl_space::non_adl_tuple_like auto&& tuple) {
  if constexpr (index == 0)
    return std::forward<decltype(tuple)>(tuple).a;
  else
    return std::forward<decltype(tuple)>(tuple).b;
}

namespace std {
template <>
struct tuple_size<adl_space::non_adl_tuple>
    : std::integral_constant<std::size_t, 2> {};
template <std::size_t index>
struct tuple_element<index, adl_space::non_adl_tuple> {
  using type = int;
};
}  // namespace std

static_assert(not xstd::tuple_like<adl_space::non_adl_tuple>);
static_assert(not xstd::tuple_like<adl_space::non_adl_tuple&>);
static_assert(not xstd::tuple_like<adl_space::non_adl_tuple&&>);
static_assert(not xstd::tuple_like<adl_space::non_adl_tuple const>);
static_assert(not xstd::tuple_like<adl_space::non_adl_tuple const&>);
static_assert(not xstd::tuple_like<adl_space::non_adl_tuple const&&>);

// Custom types that restrict their `get` functions by using qualification
// may only be considered tuple-like when properly qualified as well.
struct const_only_tuple {
  int x;
  float y;

  template <std::size_t I>
  constexpr const auto& get() const {
    if constexpr (I == 0)
      return x;
    else
      return y;
  }
};
namespace std {
template <>
struct tuple_size<const_only_tuple> : std::integral_constant<std::size_t, 2> {};
template <>
struct tuple_element<0, const_only_tuple> {
  using type = int;
};
template <>
struct tuple_element<1, const_only_tuple> {
  using type = float;
};
}  // namespace std

static_assert(not xstd::tuple_like<const_only_tuple>);
static_assert(not xstd::tuple_like<const_only_tuple&>);
static_assert(not xstd::tuple_like<const_only_tuple&&>);
static_assert(xstd::tuple_like<const_only_tuple const>);
static_assert(xstd::tuple_like<const_only_tuple const&>);
static_assert(xstd::tuple_like<const_only_tuple const&&>);

static_assert(xstd::strict_tuple_like<const_only_tuple const>);
static_assert(xstd::strict_tuple_like<const_only_tuple const&>);
static_assert(xstd::strict_tuple_like<const_only_tuple const&&>);

int main() {
  using xstd::meta::as_value;
  {
    std::tuple<int, float> tuple{1, 2};
    auto& [x, y] = tuple;
    // xstd::meta::breakpoint<decltype(get<0>(tuple))>();
    assert(get<0>(tuple) == 1);
    assert(get<1>(tuple) == 2);
    assert(x == 1);
    assert(y == 2);
    ++x, y = 1;
    assert(get<0>(tuple) == 2);
    assert(get<1>(tuple) == 1);
  }
  {
    member_gettable_tuple tuple{1, 2};
    auto& [x, y] = tuple;
    assert(tuple.get<0>() == 1);
    assert(tuple.get<1>() == 2);
    assert(x == 1);
    assert(y == 2);
    ++x, y = 1;
    assert(tuple.x == 2);
    assert(tuple.y == 1);
    assert(tuple.get<0>() == 2);
    assert(tuple.get<1>() == 1);
  }
  {
    adl_space::adl_gettable_tuple tuple{1, 2};
    auto& [x, y] = tuple;
    assert(get<0>(tuple) == 1);
    assert(get<1>(tuple) == 2);
    assert(x == 1);
    assert(y == 2);
    ++x, y = 1;
    assert(tuple.a == 2);
    assert(tuple.b == 1);
    assert(get<0>(tuple) == 2);
    assert(get<1>(tuple) == 1);
  }
  {
    constexpr adl_space::member_and_adl_gettable_tuple tuple{1};
    auto [x] = tuple;
    assert(x == 2);
  }
  {
    const const_only_tuple tuple{3, 4};
    auto& [x, y] = tuple;
    assert(tuple.get<0>() == 3);
    assert(tuple.get<1>() == 4);
    assert(x == 3);
    assert(y == 4);
  }
}
