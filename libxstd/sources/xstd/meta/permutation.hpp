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

namespace xstd::meta {

template <size_t... indices>
  requires(bounded<sizeof...(indices)>(index_list<indices...>{})) &&
              (elementwise_unique(index_list<indices...>{}))
using permutation = index_list<indices...>;

namespace detail {

template <typename type>
struct is_permutation : std::false_type {};

template <size_t... indices>
struct is_permutation<permutation<indices...>> : std::true_type {};

}  // namespace detail

template <typename type>
concept permutation_instance = detail::is_permutation<type>::value;

template <typename type, size_t n>
concept permutation_for = permutation_instance<type> && (size(type{}) == n);

template <size_t n>
consteval auto permutation_identity() {
  return index_list_from_iota<n>();
}

}  // namespace xstd::meta
