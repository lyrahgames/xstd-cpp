// Copyright © 2024 Markus Pawellek
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
#pragma once
#include <xstd/meta/list.hpp>
#include <xstd/meta/pair.hpp>
#include <xstd/meta/string.hpp>

namespace xstd::meta {

/// Checks whether each given occurs only
/// once in the template parameter pack.
///
template <typename... types>
concept no_multiplicity = elementwise_unique(type_list<types...>{});

/// The `map` template represent a compile-time map.
/// It provides an easy access/evaluation based on function overloads.
///
template <pair_instance... pairs>
  requires(no_multiplicity<typename pairs::key...>)
using map = type_list<pairs...>;

namespace detail {

template <typename type>
struct is_map : std::false_type {};

template <pair_instance... pairs>
struct is_map<map<pairs...>> : std::true_type {};

}  // namespace detail

/// Check whether given type is an instance of `map`.
///
template <typename type>
concept map_instance = detail::is_map<type>::value;

/// Get a `type_list` instance of all keys
/// contained inside the given `map` instance.
///
template <pair_instance... pairs>
consteval auto keys(map<pairs...>) {
  return type_list<typename pairs::key...>{};
}

/// Get a `type_list` instance of all values
/// contained inside the given `map` instance.
///
template <pair_instance... pairs>
consteval auto values(map<pairs...>) {
  return type_list<typename pairs::value...>{};
}

/// Template to provide constraint overload functionality.
/// Functors can be assembled like rules.
/// The function call operator will then provide the map access.
///
namespace detail::map {
template <typename... types>
struct overload : types... {
  using types::operator()...;
};
}  // namespace detail::map

/// Access a value of a contained pair by its key
/// inside a given `map` instance.
///
template <typename key, pair_instance... pairs>
consteval auto at(map<pairs...> list)
  requires(contained<key>(keys(list)))
{
  return detail::map::overload<pairs...>{}(type_tag<key>{});
}

/// Access a value of a contained pair by its key
/// given as value inside a given `map` instance.
///
template <auto key>
consteval auto at(map_instance auto list)
  requires(contained<as_type<key>>(keys(list)))
{
  return at<as_type<key>>(list);
}

/// Convenience wrapper of `at` for values of type `size_t`.
/// Access a value of a contained pair by its key
/// given as index inside a given `map` instance.
///
template <size_t n>
consteval auto at_index(map_instance auto list) {
  return at<n>(list);
}

/// Convenience wrapper of `at` for values of type `meta::string`.
/// Access a value of a contained pair by its key
/// given as string inside a given `map` instance.
///
template <string str>
consteval auto at_string(map_instance auto list) {
  return at<str>(list);
}

/// Inverse a given `type_list` instance of pairs
/// by interchanging keys and values.
/// Maps might not necessarily be bijective, i.e., invertible.
/// Thus, this function is provided for general pair lists.
///
template <pair_instance... pairs>
consteval auto inverse(type_list<pairs...>) {
  return type_list<pair<typename pairs::value, typename pairs::key>...>{};
}

}  // namespace xstd::meta
