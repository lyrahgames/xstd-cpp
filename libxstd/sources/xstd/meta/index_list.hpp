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
#include <xstd/meta/value_list.hpp>

namespace xstd::meta {

/// 'index_list' is a constrained alias of 'value_list'
/// that only contains values of type 'size_t'.
///
template <size_t... indices>
using index_list = value_list<indices...>;

namespace detail {

template <typename type>
struct is_index_list : std::false_type {};

template <size_t... indices>
struct is_index_list<index_list<indices...>> : std::true_type {};

}  // namespace detail

/// Check whether the given type is an instance of 'index_list'.
///
template <typename T>
concept index_list_instance = detail::is_index_list<T>::value;

///
/// Constructor Extensions
///

///
///
template <size_t size, size_t offset = 0>
  requires(size == 0)
consteval auto index_list_from_iota() {
  return index_list<>{};
}
//
template <size_t size, size_t offset = 0>
  requires(size > 0)
consteval auto index_list_from_iota() {
  return index_list<offset>{} + index_list_from_iota<size - 1, offset + 1>();
}

///
///
template <size_t size>
consteval auto indices_less_than() {
  return index_list_from_iota<size>();
}

///
/// Predicates
///

/// Check whether each contained value inside the
/// index list is smaller than a given integer.
///
template <size_t bound>
consteval bool bounded(index_list_instance auto list) {
  return all_of(list, []<size_t x> { return x < bound; });
}

}  // namespace xstd::meta
