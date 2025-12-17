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
#include <xstd/meta/pair.hpp>
#include <xstd/meta/string.hpp>
#include <xstd/meta/type_list.hpp>
#include <xstd/meta/value_list.hpp>

using namespace xstd;
using namespace meta;

int main() {
  // Simple Tests for `pair_instance` Concept
  //
  static_assert(!pair_instance<int>);
  static_assert(!pair_instance<type_list<>>);
  static_assert(!pair_instance<type_list<int, float>>);
  static_assert(!pair_instance<value_list<>>);
  static_assert(!pair_instance<value_list<-1, 1.23f>>);
  //
  static_assert(pair_instance<pair<int, float>>);
  static_assert(pair_instance<pair<float, float>>);
  static_assert(pair_instance<pair<czstring, std::string>>);
  static_assert(pair_instance<pair<std::string_view, std::string>>);
  //
  static_assert(pair_instance<pair<as_type<-1>, float>>);
  static_assert(pair_instance<pair<int, as_type<"integer"_xs>>>);

  // Invocation of Pairs
  //
  for_each(type_list<                                  //
               pair<int, float>,                       //
               pair<as_type<-1>, float>,               //
               pair<int, as_type<-1>>,                 //
               pair<as_type<-1>, as_type<"name"_xs>>,  //
               pair<type_tag<int>, type_tag<float>>>{},
           []<typename p> {
             using k = typename p::key;
             using v = typename p::value;

             // Pairs can be invoked by their key type wrapped in `type_tag`.
             // This wrapping is unconditional.
             //
             static_assert(std::invocable<p, type_tag<k>>);

             // The invocation returns the `value` type as value interpretation,
             // i.e., constexpr values are returned as the actual values.
             //
             static_assert(as_value<v> == std::invoke(p{}, type_tag<k>{}));

             // Key and value can also be accessed by using functions directly.
             //
             static_assert(as_value<k> == key(p{}));
             static_assert(as_value<v> == value(p{}));
           });
}
