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
#include <array>
//
#include <xstd/functional/tuple.hpp>

using namespace xstd;

static_assert(generic_tuple<std::tuple<>>);
static_assert(generic_tuple<std::tuple<int>>);
static_assert(generic_tuple<std::tuple<int, float>>);
//
static_assert(generic_tuple<std::array<int, 0>>);
static_assert(generic_tuple<std::array<int, 5>>);
static_assert(generic_tuple<std::array<float, 0>>);
static_assert(generic_tuple<std::array<float, 5>>);

static_assert(type_list_from<std::tuple<>>() == meta::type_list<>{});
static_assert(type_list_from<std::tuple<int>>() == meta::type_list<int>{});
static_assert(type_list_from<std::tuple<int&>>() == meta::type_list<int&>{});
static_assert(type_list_from<std::tuple<int&, float>>() ==
              meta::type_list<int&, float>{});
//
static_assert(type_list_from<std::array<int, 0>>() == meta::type_list<>{});
static_assert(type_list_from<std::array<int, 5>>() ==
              meta::type_list<int, int, int, int, int>{});
