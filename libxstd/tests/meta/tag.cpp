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
#include <thread>
//
#include <xstd/meta/tag.hpp>

using namespace xstd::meta;

/// All instances of `type_tag` fulfill `generic_tag`,
/// `generic_type_tag`, and `type_tag_instance`.
///
static_assert(type_tag_instance<type_tag<int>>);
static_assert(type_tag_instance<type_tag<void>>);
static_assert(type_tag_instance<type_tag<float>>);
static_assert(!type_tag_instance<value_tag<-1>>);
static_assert(!type_tag_instance<value_tag<'c'>>);
static_assert(!type_tag_instance<value_tag<1.23f>>);
static_assert(!type_tag_instance<int>);
static_assert(!type_tag_instance<void>);
static_assert(!type_tag_instance<float>);
//
static_assert(generic_tag<type_tag<int>>);
static_assert(generic_tag<type_tag<void>>);
static_assert(generic_tag<type_tag<float>>);
//
static_assert(generic_type_tag<type_tag<int>>);
static_assert(generic_type_tag<type_tag<void>>);
static_assert(generic_type_tag<type_tag<float>>);

/// All `value_tag` instances fulfill `generic_tag`,
/// `generic_value_tag`, and `value_tag_instance`.
///
static_assert(value_tag_instance<value_tag<-1>>);
static_assert(value_tag_instance<value_tag<'c'>>);
static_assert(value_tag_instance<value_tag<1.23f>>);
static_assert(!value_tag_instance<type_tag<int>>);
static_assert(!value_tag_instance<type_tag<void>>);
static_assert(!value_tag_instance<type_tag<float>>);
//
static_assert(generic_tag<value_tag<-1>>);
static_assert(generic_tag<value_tag<1.23f>>);
static_assert(generic_tag<value_tag<'c'>>);
//
static_assert(generic_value_tag<value_tag<-1>>);
static_assert(generic_value_tag<value_tag<1.23f>>);
static_assert(generic_value_tag<value_tag<'c'>>);

/// `std::integral_constant` instances fulfill
/// `generic_tag` and `generic_value_tag`.
///
static_assert(generic_tag<std::true_type>);
static_assert(generic_tag<std::false_type>);
static_assert(generic_value_tag<std::true_type>);
static_assert(generic_value_tag<std::false_type>);
static_assert(!type_tag_instance<std::true_type>);
static_assert(!type_tag_instance<std::false_type>);
static_assert(!value_tag_instance<std::true_type>);
static_assert(!value_tag_instance<std::false_type>);
//
static_assert(generic_tag<std::integral_constant<int, -1>>);
static_assert(generic_tag<std::integral_constant<size_t, 1>>);
static_assert(generic_tag<std::integral_constant<float, 1.23f>>);
static_assert(generic_value_tag<std::integral_constant<int, -1>>);
static_assert(generic_value_tag<std::integral_constant<size_t, 1>>);
static_assert(generic_value_tag<std::integral_constant<float, 1.23f>>);
static_assert(!type_tag_instance<std::integral_constant<int, -1>>);
static_assert(!type_tag_instance<std::integral_constant<size_t, 1>>);
static_assert(!type_tag_instance<std::integral_constant<float, 1.23f>>);
static_assert(!value_tag_instance<std::integral_constant<int, -1>>);
static_assert(!value_tag_instance<std::integral_constant<size_t, 1>>);
static_assert(!value_tag_instance<std::integral_constant<float, 1.23f>>);

/// Other standard tag types, such as `std::in_place_type_t`, fulfill
/// `generic_tag` but not `generic_value_tag` or `generic_type_tag`
/// as do not offer access to their wrapped value or type.
///
static_assert(generic_tag<std::in_place_index_t<0>>);
static_assert(generic_tag<std::in_place_index_t<1>>);
static_assert(!generic_value_tag<std::in_place_index_t<0>>);
static_assert(!generic_value_tag<std::in_place_index_t<1>>);
static_assert(!generic_type_tag<std::in_place_index_t<0>>);
static_assert(!generic_type_tag<std::in_place_index_t<1>>);
//
static_assert(generic_tag<std::in_place_type_t<int>>);
static_assert(generic_tag<std::in_place_type_t<float>>);
static_assert(!generic_type_tag<std::in_place_type_t<int>>);
static_assert(!generic_type_tag<std::in_place_type_t<float>>);
static_assert(!generic_value_tag<std::in_place_type_t<int>>);
static_assert(!generic_value_tag<std::in_place_type_t<float>>);
//
static_assert(generic_tag<std::integer_sequence<std::size_t, 0, 2, 1>>);
static_assert(!generic_type_tag<std::integer_sequence<std::size_t, 0, 2, 1>>);
static_assert(!generic_value_tag<std::integer_sequence<std::size_t, 0, 2, 1>>);

/// The usage of `as_value` with `value_tag` instances
/// returns the wrapped value directly.
///
static_assert(as_value<value_tag<1.23f>> == 1.23f);
static_assert(as_value<value_tag<-1>> == -1);
static_assert(as_value<value_tag<'c'>> == 'c');

/// Using `as_value` with other tag types, results in
/// default constructed values from those tag types.
///
static_assert(as_value<type_tag<float>> == type_tag<float>{});
static_assert(as_value<type_tag<int>> == type_tag<int>{});
static_assert(as_value<type_tag<std::thread>> == type_tag<std::thread>{});
static_assert(as_value<std::true_type> == std::true_type{});
static_assert(as_value<std::false_type> == std::false_type{});
static_assert(as_value<std::in_place_type_t<int>> == std::in_place_type<int>);
static_assert(as_value<std::in_place_index_t<1>> == std::in_place_index<1>);

/// Using `as_value` with all other types results
/// in default constructed values whose types are
/// `type_tag` instances that wrap the given type.
///
static_assert(as_value<float> == type_tag<float>{});
static_assert(as_value<int> == type_tag<int>{});
static_assert(as_value<std::string> == type_tag<std::string>{});
static_assert(as_value<std::thread> == type_tag<std::thread>{});

/// The usage of `as_type` with instances of `type_tag`
/// returns the wrapped type directly.
///
static_assert(equal<as_type<type_tag<int>{}>, int>);
static_assert(equal<as_type<type_tag<float>{}>, float>);
static_assert(equal<as_type<type_tag<std::string>{}>, std::string>);
static_assert(equal<as_type<type_tag<std::thread>{}>, std::thread>);

/// Using `as_type` with values whose types are other tag types
/// results in the types of those values by using `decltype`.
///
static_assert(equal<as_type<value_tag<1.23f>{}>, value_tag<1.23f>>);
static_assert(equal<as_type<value_tag<-1>{}>, value_tag<-1>>);
static_assert(equal<as_type<value_tag<'c'>{}>, value_tag<'c'>>);
static_assert(equal<as_type<std::true_type{}>, std::true_type>);
static_assert(equal<as_type<std::false_type{}>, std::false_type>);
static_assert(equal<as_type<std::in_place_index<0>>, std::in_place_index_t<0>>);
static_assert(
    equal<as_type<std::in_place_type<int>>, std::in_place_type_t<int>>);

/// Using `as_type` on any other value results in an
/// instance of `value_tag` that wraps the given value.
///
static_assert(equal<as_type<1.23f>, value_tag<1.23f>>);
static_assert(equal<as_type<-1>, value_tag<-1>>);
static_assert(equal<as_type<'c'>, value_tag<'c'>>);

/// Using of `as_value` and then `as_type`:
///
static_assert(equal<int, as_type<as_value<int>>>);
static_assert(equal<float, as_type<as_value<float>>>);
static_assert(equal<std::string, as_type<as_value<std::string>>>);
static_assert(equal<std::thread, as_type<as_value<std::thread>>>);
//
static_assert(equal<value_tag<1.23f>, as_type<as_value<value_tag<1.23f>>>>);
static_assert(equal<value_tag<-1>, as_type<as_value<value_tag<-1>>>>);
static_assert(equal<value_tag<'c'>, as_type<as_value<value_tag<'c'>>>>);
//
static_assert(equal<int, as_type<as_value<type_tag<int>>>>);
static_assert(equal<float, as_type<as_value<type_tag<float>>>>);
static_assert(equal<std::string, as_type<as_value<type_tag<std::string>>>>);
static_assert(equal<std::thread, as_type<as_value<type_tag<std::thread>>>>);
//
static_assert(equal<std::true_type, as_type<as_value<std::true_type>>>);
static_assert(equal<std::in_place_type_t<int>,
                    as_type<as_value<std::in_place_type_t<int>>>>);

/// Using of `as_type` and then `as_value`:
///
static_assert(1.23f == as_value<as_type<1.23f>>);
static_assert(-1 == as_value<as_type<-1>>);
static_assert('c' == as_value<as_type<'c'>>);
//
static_assert(type_tag<float>{} == as_value<as_type<type_tag<float>{}>>);
static_assert(as_value<float> == as_value<as_type<type_tag<float>{}>>);
static_assert(as_value<int> == as_value<as_type<type_tag<int>{}>>);
static_assert(as_value<std::string> ==
              as_value<as_type<type_tag<std::string>{}>>);
static_assert(as_value<std::thread> ==
              as_value<as_type<type_tag<std::thread>{}>>);
//
static_assert(1.23f == as_value<as_type<value_tag<1.23f>{}>>);
static_assert(-1 == as_value<as_type<value_tag<-1>{}>>);
static_assert('c' == as_value<as_type<value_tag<'c'>{}>>);
//
static_assert(std::in_place_type<int> ==
              as_value<as_type<std::in_place_type<int>>>);
static_assert(std::in_place_index<1> ==
              as_value<as_type<std::in_place_index<1>>>);

/// Use `as_value` as utility to compare types with `operator==`.
///
static_assert(as_value<float> == type_tag<float>{});
static_assert(as_value<float> != as_value<double>);
static_assert(as_value<int> != as_value<unsigned int>);
static_assert(as_value<int> == as_value<int>);
static_assert(as_value<int> != as_value<int&>);

/// Use `as_signature` as utility to test values for
/// strict equality, i.e., equality of type and value.
///
static_assert(uint32_t{1} == uint64_t{1});
static_assert(as_signature<uint32_t{1}> != as_signature<uint64_t{1}>);
static_assert(as_signature<uint64_t{1}> == as_signature<uint64_t{1}>);
static_assert(as_signature<true> == value_tag<true>{});
static_assert(as_signature<1> != as_signature<1ull>);
static_assert(1 == 1ull);

int main() {}
