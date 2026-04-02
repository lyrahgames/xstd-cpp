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
import xstd;

using xstd::meta::index_list;
using xstd::meta::iota;
using xstd::meta::permutation;
using xstd::meta::permutation_for;
using xstd::meta::permutation_instance;

int main() {
  static_assert(permutation_for<index_list<>, 0>);
  //
  static_assert(permutation_for<index_list<0>, 1>);
  static_assert(!permutation_for<index_list<1>, 1>);
  static_assert(!permutation_for<index_list<2>, 1>);
  static_assert(!permutation_for<index_list<0, 1>, 1>);
  static_assert(!permutation_for<index_list<1, 2, 0>, 1>);
  //
  static_assert(permutation_for<index_list<0, 1>, 2>);
  static_assert(permutation_for<index_list<1, 0>, 2>);
  static_assert(!permutation_for<index_list<0>, 2>);
  static_assert(!permutation_for<index_list<1, 2>, 2>);
  static_assert(!permutation_for<index_list<0, 2>, 2>);
  static_assert(!permutation_for<index_list<0, 2, 1>, 2>);
  //
  static_assert(permutation_for<index_list<0, 1, 2>, 3>);
  static_assert(permutation_for<index_list<0, 2, 1>, 3>);
  static_assert(permutation_for<index_list<1, 0, 2>, 3>);
  static_assert(permutation_for<index_list<1, 2, 0>, 3>);
  static_assert(permutation_for<index_list<2, 1, 0>, 3>);
  static_assert(permutation_for<index_list<2, 0, 1>, 3>);
  static_assert(permutation_for<index_list<2, 0, 1>, 3>);
  static_assert(!permutation_for<index_list<0>, 3>);
  static_assert(!permutation_for<index_list<0, 1>, 3>);
  static_assert(!permutation_for<index_list<0, 1, 2, 3>, 3>);
  static_assert(!permutation_for<index_list<3, 1, 2>, 3>);

  static_assert(iota<0>() == permutation<>{});
  static_assert(iota<1>() == permutation<0>{});
  static_assert(iota<2>() == permutation<0, 1>{});
  static_assert(iota<3>() == permutation<0, 1, 2>{});
  static_assert(iota<4>() == permutation<0, 1, 2, 3>{});
  static_assert(iota<5>() == permutation<0, 1, 2, 3, 4>{});
  static_assert(iota<6>() == permutation<0, 1, 2, 3, 4, 5>{});
}
