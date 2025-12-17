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
#include <xstd/meta/string.hpp>
#include <xstd/meta/value_list.hpp>

namespace xstd::meta {

/// `string_list` is a constrained alias of `value list`
/// that only contains values of type `meta::string`.
///
template <string... strings>
using string_list = value_list<strings...>;

namespace detail {

template <typename type>
struct is_string_list : std::false_type {};

template <string... strings>
struct is_string_list<string_list<strings...>> : std::true_type {};

}  // namespace detail

/// Check whether the given type is an instance of `string_list`.
///
template <typename list>
concept string_list_instance = detail::is_string_list<list>::value;

}  // namespace xstd::meta
