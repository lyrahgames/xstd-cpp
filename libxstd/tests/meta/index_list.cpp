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
#include <xstd/meta/index_list.hpp>

using xstd::meta::index_list;
using xstd::meta::index_list_from_iota;

static_assert(index_list_from_iota<0>() == index_list<>{});
static_assert(index_list_from_iota<1>() == index_list<0>{});
static_assert(index_list_from_iota<2>() == index_list<0, 1>{});
static_assert(index_list_from_iota<3>() == index_list<0, 1, 2>{});
static_assert(index_list_from_iota<4>() == index_list<0, 1, 2, 3>{});
//
static_assert(index_list_from_iota<0, 2>() == index_list<>{});
static_assert(index_list_from_iota<1, 2>() == index_list<2>{});
static_assert(index_list_from_iota<2, 2>() == index_list<2, 3>{});
static_assert(index_list_from_iota<3, 2>() == index_list<2, 3, 4>{});
static_assert(index_list_from_iota<4, 2>() == index_list<2, 3, 4, 5>{});

static_assert(bounded<0>(index_list<>{}));
static_assert(bounded<1>(index_list<>{}));
static_assert(bounded<2>(index_list<>{}));
//
static_assert(!bounded<0>(index_list<0>{}));
static_assert(bounded<1>(index_list<0>{}));
static_assert(bounded<2>(index_list<0>{}));
//
static_assert(!bounded<0>(index_list<1>{}));
static_assert(!bounded<1>(index_list<1>{}));
static_assert(bounded<2>(index_list<1>{}));
//
static_assert(bounded<4>(index_list<1, 2, 3>{}));

int main() {}
