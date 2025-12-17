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
#include <xstd/meta/index_list.hpp>
#include <xstd/meta/pair.hpp>
#include <xstd/meta/type_list.hpp>
#include <xstd/meta/value_list.hpp>

namespace xstd::meta {

/// Generate a `type_list` instance out of a given `value_list` instance.
/// The transformation uses the `as_type` meta function to transform types.
/// Thus, values wrapped by `type_tag` are unwrapped to types,
/// 'constexpr' values are wrapped by `value_tag`,
/// and all other tag types are default constructed.
///
template <auto... values>
consteval auto type_list_from(value_list<values...>) {
  return type_list<as_type<values>...>{};
}

/// Generate a `value_list` instance out of a given `type_list` instance.
/// The transformation uses the `as_value` meta function to transform types.
/// Thus, values wrapped by `value_tag` are unwrapped,
/// values of tag types will transformed by `decltype`,
/// and values of all other types are wrapped by `type_tag`.
///
template <typename... types>
consteval auto value_list_from(type_list<types...>) {
  return value_list<as_value<types>...>{};
}

/// `zip` takes two instances of `type_list` or `value_list`
/// that have the same size and returns a `type_list` instance
/// of pairs that map elements of the first list to elements
/// of the second list in their given order.
///
template <typename... x, typename... y, size_t... indices>
consteval auto zip(type_list<x...> keys,
                   type_list<y...> values,
                   index_list<indices...>) {
  return type_list<pair<as_type<element<indices>(keys)>,
                        as_type<element<indices>(values)>>...>{};
}
//
template <typename... x, typename... y>
consteval auto zip(type_list<x...> keys, type_list<y...> values)
  requires(sizeof...(x) == sizeof...(y))
{
  return zip(keys, values, index_list_from_iota<sizeof...(x)>());
}
//
consteval auto zip(value_list_instance auto keys,
                   type_list_instance auto values) {
  return zip(type_list_from(keys), values);
}
//
consteval auto zip(type_list_instance auto keys,
                   value_list_instance auto values) {
  return zip(keys, type_list_from(values));
}
//
consteval auto zip(value_list_instance auto keys,
                   value_list_instance auto values) {
  return zip(type_list_from(keys), type_list_from(values));
}

/// Return a list of pairs that contain the index of
/// an element as key and the element itself as value.
///
consteval auto pairs_from(type_list_instance auto list) {
  return zip(index_list_from_iota<size(list)>(), list);
}
//
consteval auto pairs_from(value_list_instance auto list) {
  return zip(index_list_from_iota<size(list)>(), list);
}

}  // namespace xstd::meta
