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
#include <xstd/meta/map.hpp>
#include <xstd/meta/name_list.hpp>

using namespace xstd;
using namespace meta;

int main() {
  // Simple Tests for Multiplicity
  //
  static_assert(no_multiplicity<>);
  static_assert(no_multiplicity<int>);
  static_assert(no_multiplicity<float>);
  static_assert(no_multiplicity<int, float>);
  static_assert(!no_multiplicity<int, int>);
  static_assert(!no_multiplicity<float, float>);

  // Simple Tests for `map_instance` Concept
  //
  static_assert(map_instance<map<>>);
  static_assert(map_instance<map<pair<int, float>>>);
  static_assert(map_instance<map<pair<int, as_type<"integer"_xs>>>>);
  static_assert(map_instance<                           //
                map<                                    //
                    pair<int, float>,                   //
                    pair<float, as_type<"integer"_xs>>  //
                    >                                   //
                >);
  //
  // `map` is only an alias for `type_list`.
  static_assert(map_instance<type_list<>>);
  static_assert(map_instance<type_list<pair<int, float>>>);
  // Constraint Failure (Compile Error when used with `map`)
  static_assert(!map_instance<                        //
                type_list<                            //
                    pair<int, float>,                 //
                    pair<int, as_type<"integer"_xs>>  //
                    >                                 //
                >);
  //
  static_assert(!map_instance<int>);
  static_assert(!map_instance<type_list<int, float>>);
  static_assert(!map_instance<value_list<>>);
  static_assert(!map_instance<value_list<-1, 1.23f>>);
  //
  static_assert(!map_instance<pair<int, float>>);
  static_assert(!map_instance<pair<float, float>>);
  static_assert(!map_instance<pair<czstring, std::string>>);
  static_assert(!map_instance<pair<std::string_view, std::string>>);
  static_assert(!map_instance<pair<as_type<-1>, float>>);
  static_assert(!map_instance<pair<int, as_type<"integer"_xs>>>);

  // Map Access Tests
  //
  {
    constexpr auto m = map<                     //
        pair<int, float>,                       //
        pair<float, as_type<"integer"_xs>>,     //
        pair<as_type<"name"_xs>, as_type<-1>>,  //
        pair<as_type<size_t{0}>, int>>{};

    static_assert(
        keys(m) ==
        type_list<int, float, as_type<"name"_xs>, as_type<size_t{0}>>{});
    static_assert(values(m) ==
                  type_list<float, as_type<"integer"_xs>, as_type<-1>, int>{});

    static_assert(at<int>(m) == as_value<float>);
    static_assert(at<float>(m) == "integer"_xs);
    static_assert(at_string<"name">(m) == -1);
    static_assert(at_index<0>(m) == as_value<int>);

    // The inverse representation can always be computed
    // but might not represent a map anymore.
    //
    constexpr auto im = inverse(m);
    static_assert(map_instance<decltype(inverse(m))>);

    // Map Access for the Inverse in case of a bijective map.
    //
    static_assert(at<float>(im) == as_value<int>);
    static_assert(at_string<"integer">(im) == as_value<float>);
    static_assert(at<-1>(im) == "name"_xs);
    static_assert(at<int>(im) == size_t{0});

    static_assert(zip(keys(m), values(m)) == m);
    static_assert(zip(values(m), keys(m)) == im);
  }

  // Test generation of maps out of lists.
  //
  {
    constexpr auto names = name_list<"first", "second", "third", "fourth">{};

    static_assert(element<0>(names) == "first"_xs);
    static_assert(element<1>(names) == "second"_xs);
    static_assert(element<2>(names) == "third"_xs);
    static_assert(element<3>(names) == "fourth"_xs);

    constexpr auto name_by_index = pairs_from(names);

    static_assert(at_index<0>(name_by_index) == "first"_xs);
    static_assert(at_index<1>(name_by_index) == "second"_xs);
    static_assert(at_index<2>(name_by_index) == "third"_xs);
    static_assert(at_index<3>(name_by_index) == "fourth"_xs);

    constexpr auto index_by_name = inverse(name_by_index);

    static_assert(at_string<"first">(index_by_name) == size_t{0});
    static_assert(at_string<"second">(index_by_name) == size_t{1});
    static_assert(at_string<"third">(index_by_name) == size_t{2});
    static_assert(at_string<"fourth">(index_by_name) == size_t{3});
  }
}
