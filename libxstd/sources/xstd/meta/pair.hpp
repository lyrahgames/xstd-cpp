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
#include <xstd/meta/tag.hpp>

namespace xstd::meta {

/// The `pair` template is used to represent
/// key-value pairs inside maps and multi-maps.
/// To represent actual constexpr values,
/// the `value_tag` template is used.
///
template <typename k, typename v>
struct pair {
  using key = k;
  using value = v;
  // We need to unconditionally wrap `key` in `type_tag`.
  // `key` might be another instance of `type_tag`.
  consteval auto operator()(type_tag<key>) noexcept { return as_value<value>; }
};

namespace detail {

template <typename type>
struct is_pair : std::false_type {};

template <typename key, typename value>
struct is_pair<pair<key, value>> : std::true_type {};

}  // namespace detail

/// Check whether a given type is an instance of `pair`.
///
template <typename type>
concept pair_instance = detail::is_pair<type>::value;

/// Access the key of a given 'pair' instance.
/// Standard types are returned wrapped by `type_tag`.
/// Values wrapped by `value_tag` are unwrapped returned directly.
/// For other tag types, a default constructed value is returned.
///
template <typename k, typename v>
consteval auto key(pair<k, v>) {
  return as_value<k>;
}

/// Access the value of a given `pair` instance.
/// Standard types are returned wrapped by `type_tag`.
/// Values wrapped by `value_tag` are unwrapped returned directly.
/// For other tag types, a default constructed value is returned.
///
template <typename k, typename v>
consteval auto value(pair<k, v>) {
  return as_value<v>;
}

}  // namespace xstd::meta
