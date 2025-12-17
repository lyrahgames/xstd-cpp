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
#include <print>
//
#include <xstd/meta/type_list.hpp>

// To access functions dealing with instantiations of 'type_list',
// we use argument-dependent lookup (ADL) and simply make the template
// itself available in the current scope.
//
using xstd::meta::type_list;

// For the following tests, we need to often check whether
// a certain type is an instance of the 'type_list' template.
//
using xstd::meta::type_list_instance;

// To simplify the checks for type equality,
// we make the 'equal' predicate available in the current scope.
//
using xstd::meta::equal;

using xstd::meta::as_type;
using xstd::meta::as_value;

// Provide a total order predicate for types.
// This is only an example based on the type's size.
// Of course, also alignment and other infos could be used.
//
constexpr auto less = []<typename x, typename y> {
  return sizeof(x) <= sizeof(y);
};

// Check whether a given type is an instance of the 'type_list' template.
//
static_assert(type_list_instance<type_list<>>);
static_assert(type_list_instance<type_list<int>>);
static_assert(type_list_instance<type_list<char>>);
static_assert(type_list_instance<type_list<int, char>>);
static_assert(type_list_instance<type_list<int, char, float>>);
static_assert(type_list_instance<  //
              type_list<int, type_list<char, float>, type_list<>>>);
//
static_assert(!type_list_instance<int>);
static_assert(!type_list_instance<float>);
static_assert(!type_list_instance<type_list<>&>);
static_assert(!type_list_instance<const type_list<>>);
static_assert(!type_list_instance<const type_list<>&>);

// Get the number of types inside a type list.
//
static_assert(size(type_list<>{}) == 0);
static_assert(size(type_list<int>{}) == 1);
static_assert(size(type_list<char>{}) == 1);
static_assert(size(type_list<int, char>{}) == 2);
static_assert(size(type_list<int, char, double>{}) == 3);

// Check whether a 'type_list' instance contains no types.
//
static_assert(empty(type_list<>{}));
static_assert(!empty(type_list<int>{}));
static_assert(!empty(type_list<int, char>{}));
static_assert(!empty(type_list<int, char, double>{}));

// Check whether a specific condition, given by a predicate,
// holds for all contained types in a 'type_list' instance.
//
constexpr auto correct_alignment = []<typename x> { return alignof(x) >= 4; };
static_assert(all_of(type_list<>{}, correct_alignment));
static_assert(all_of(type_list<float>{}, correct_alignment));
static_assert(!all_of(type_list<char>{}, correct_alignment));
static_assert(all_of(type_list<float, double>{}, correct_alignment));
static_assert(!all_of(type_list<float, char>{}, correct_alignment));
static_assert(!all_of(type_list<short, char>{}, correct_alignment));

// Check whether a 'type_list' instance contains a type
// that fulfills a specific condition, given by a predicate.
//
static_assert(!any_of(type_list<>{}, correct_alignment));
static_assert(!any_of(type_list<char>{}, correct_alignment));
static_assert(any_of(type_list<float>{}, correct_alignment));
static_assert(any_of(type_list<double>{}, correct_alignment));
static_assert(!any_of(type_list<char, short>{}, correct_alignment));
static_assert(any_of(type_list<char, float>{}, correct_alignment));
static_assert(any_of(type_list<float, double>{}, correct_alignment));

// Check if a 'type_list' instance contains a specific type.
//
static_assert(!contained<int>(type_list<>{}));
static_assert(!contained<char>(type_list<>{}));
static_assert(contained<int>(type_list<int>{}));
static_assert(!contained<int>(type_list<char>{}));
static_assert(contained<int>(type_list<int, char>{}));
static_assert(contained<char>(type_list<int, char>{}));
static_assert(!contained<float>(type_list<int, char>{}));
//
static_assert(!contained(type_list<>{}, type_list<int>{}));
static_assert(!contained(type_list<>{}, type_list<char>{}));
static_assert(contained(type_list<int>{}, type_list<int>{}));
static_assert(!contained(type_list<char>{}, type_list<int>{}));
static_assert(contained(type_list<int, char>{}, type_list<int>{}));
static_assert(contained(type_list<int, char>{}, type_list<char>{}));
static_assert(!contained(type_list<int, char>{}, type_list<float>{}));

// Access types of a 'type_list' instance by their index.
//
static_assert(element<0>(type_list<int>{}) == as_value<int>);
static_assert(element<0>(type_list<int, char>{}) == as_value<int>);
static_assert(element<1>(type_list<int, char>{}) == as_value<char>);
static_assert(element<0>(type_list<double, int, char>{}) == as_value<double>);
static_assert(element<1>(type_list<double, int, char>{}) == as_value<int>);
static_assert(element<2>(type_list<double, int, char>{}) == as_value<char>);

//
//
static_assert(index<int>(type_list<int>{}) == 0);
static_assert(index<int>(type_list<float, int>{}) == 1);

// Get the wrapped first element of a 'type_list' instance.
//
static_assert(front(type_list<int>{}) == as_value<int>);
static_assert(front(type_list<char, int>{}) == as_value<char>);
static_assert(front(type_list<double, char, int>{}) == as_value<double>);
//
// You can also use the '*' operator for this.
//
static_assert(*type_list<int>{} == type_list<int>{});
static_assert(*type_list<char, int>{} == type_list<char>{});
static_assert(*type_list<double, char, int>{} == type_list<double>{});

// Get the wrapped last element of a 'type_list' instance.
//
static_assert(back(type_list<int>{}) == as_value<int>);
static_assert(back(type_list<char, int>{}) == as_value<int>);
static_assert(back(type_list<double, char, int>{}) == as_value<int>);

// Add a type to the front of a 'type_list' instance.
//
static_assert(push_front<int>(type_list<>{}) == type_list<int>{});
static_assert(push_front<char>(type_list<int>{}) == type_list<char, int>{});
static_assert(push_front<float>(type_list<char, int>{}) ==
              type_list<float, char, int>{});
static_assert(push_front<type_list<>>(type_list<int>{}) ==
              type_list<type_list<>, int>{});

// Add a type to the back of a 'type_list' instance.
//
static_assert(push_back<int>(type_list<>{}) == type_list<int>{});
static_assert(push_back<char>(type_list<int>{}) == type_list<int, char>{});
static_assert(push_back<float>(type_list<char, int>{}) ==
              type_list<char, int, float>{});
static_assert(push_back<type_list<>>(type_list<int>{}) ==
              type_list<int, type_list<>>{});

// Concatenate 'type_list' instances.
//
static_assert(concat(type_list<>{}, type_list<>{}) == type_list<>{});
static_assert(concat(type_list<int>{}, type_list<>{}) == type_list<int>{});
static_assert(concat(type_list<>{}, type_list<int>{}) == type_list<int>{});
static_assert(concat(type_list<int>{}, type_list<char>{}) ==
              type_list<int, char>{});
static_assert(concat(type_list<char>{}, type_list<int>{}) ==
              type_list<char, int>{});
static_assert(concat(type_list<char, float>{}, type_list<int>{}) ==
              type_list<char, float, int>{});
static_assert(concat(type_list<float>{}, type_list<char, int>{}) ==
              type_list<float, char, int>{});
//
// You can also use the '+' operator for this.
//
static_assert(type_list<>{} + type_list<>{} == type_list<>{});
static_assert(type_list<int>{} + type_list<>{} == type_list<int>{});
static_assert(type_list<>{} + type_list<int>{} == type_list<int>{});
static_assert(type_list<int>{} + type_list<char>{} == type_list<int, char>{});
static_assert(type_list<char>{} + type_list<int>{} == type_list<char, int>{});
static_assert(type_list<char, float>{} + type_list<int>{} ==
              type_list<char, float, int>{});
static_assert(type_list<float>{} + type_list<char, int>{} ==
              type_list<float, char, int>{});

// Pop an element from the front of a 'type_list' instance.
//
static_assert(pop_front(type_list<int>{}) == type_list<>{});
static_assert(pop_front(type_list<char, int>{}) == type_list<int>{});
static_assert(pop_front(type_list<float, char, int>{}) ==
              type_list<char, int>{});
//
// You can also use '--' operator for this.
//
static_assert(--type_list<int>{} == type_list<>{});
static_assert(--type_list<char, int>{} == type_list<int>{});
static_assert(--type_list<float, char, int>{} == type_list<char, int>{});

// Pop an element from the back of a 'type_list' instance.
//
static_assert(pop_back(type_list<int>{}) == type_list<>{});
static_assert(pop_back(type_list<char, int>{}) == type_list<char>{});
static_assert(pop_back(type_list<float, char, int>{}) ==
              type_list<float, char>{});
//
// You can also use the '--' operator for this.
//
static_assert(type_list<int> {} --== type_list<>{});
static_assert(type_list<char, int> {} --== type_list<char>{});
static_assert(type_list<float, char, int> {} --== type_list<float, char>{});

// Reverse the order of types in a 'type_list' instance.
//
static_assert(reverse(type_list<>{}) == type_list<>{});
static_assert(reverse(type_list<int>{}) == type_list<int>{});
static_assert(reverse(type_list<char>{}) == type_list<char>{});
static_assert(reverse(type_list<int, char>{}) == type_list<char, int>{});
static_assert(reverse(type_list<char, int>{}) == type_list<int, char>{});
static_assert(reverse(type_list<char, int, float>{}) ==
              type_list<float, int, char>{});
//
// You can also use the '~' operator for this.
//
static_assert(~type_list<>{} == type_list<>{});
static_assert(~type_list<int>{} == type_list<int>{});
static_assert(~type_list<char>{} == type_list<char>{});
static_assert(~type_list<int, char>{} == type_list<char, int>{});
static_assert(~type_list<char, int>{} == type_list<int, char>{});
static_assert(~type_list<char, int, float>{} == type_list<float, int, char>{});

// Insert types at a specific position in a 'type_list' instance.
//
static_assert(insert<0, int>(type_list<>{}) == type_list<int>{});
static_assert(insert<0, char>(type_list<int>{}) == type_list<char, int>{});
static_assert(insert<1, char>(type_list<int>{}) == type_list<int, char>{});
static_assert(insert<0, float>(type_list<int, char>{}) ==
              type_list<float, int, char>{});
static_assert(insert<1, float>(type_list<int, char>{}) ==
              type_list<int, float, char>{});
static_assert(insert<2, float>(type_list<int, char>{}) ==
              type_list<int, char, float>{});

// Insert types in a 'type_list' instance by using a predicate.
//
static_assert(insert<int>(type_list<>{}, less) == type_list<int>{});
static_assert(insert<char>(type_list<>{}, less) == type_list<char>{});
static_assert(insert<int>(type_list<char>{}, less) == type_list<char, int>{});
static_assert(insert<unsigned>(type_list<char, int>{}, less) ==
              type_list<char, unsigned, int>{});
static_assert(insert<short>(type_list<char, int>{}, less) ==
              type_list<char, short, int>{});
static_assert(insert<double>(type_list<char, int>{}, less) ==
              type_list<char, int, double>{});

// Remove types at a specific position in a 'type_list' instance.
//
static_assert(remove<0>(type_list<int>{}) == type_list<>{});
static_assert(remove<0>(type_list<int, char>{}) == type_list<char>{});
static_assert(remove<1>(type_list<int, char>{}) == type_list<int>{});
static_assert(remove<0>(type_list<float, int, char>{}) ==
              type_list<int, char>{});
static_assert(remove<1>(type_list<float, int, char>{}) ==
              type_list<float, char>{});
static_assert(remove<2>(type_list<float, int, char>{}) ==
              type_list<float, int>{});

// Remove types in a 'type_list' instance by using a predicate.
//
constexpr auto too_big = []<typename x> { return sizeof(x) > 2; };
static_assert(remove_all(type_list<>{}, too_big) == type_list<>{});
static_assert(remove_all(type_list<char>{}, too_big) == type_list<char>{});
static_assert(remove_all(type_list<int>{}, too_big) == type_list<>{});
static_assert(remove_all(type_list<char, int>{}, too_big) == type_list<char>{});
static_assert(remove_all(type_list<int, char>{}, too_big) == type_list<char>{});
static_assert(remove_all(type_list<int, unsigned>{}, too_big) == type_list<>{});
static_assert(remove_all(type_list<short, char>{}, too_big) ==
              type_list<short, char>{});
static_assert(remove_all(type_list<int, unsigned, char, short>{}, too_big) ==
              type_list<char, short>{});
//
// static_assert(remove(type_list<char, short, int, unsigned, float>{},
//                      []<size_t i, typename type> { return (i % 2) != 0; }) ==
//               type_list<char, int, float>{});

// Trim the front.
//
static_assert(trim_front<0>(type_list<>{}) == type_list<>{});
static_assert(trim_front<0>(type_list<int>{}) == type_list<int>{});
static_assert(trim_front<1>(type_list<int>{}) == type_list<>{});
static_assert(trim_front<0>(type_list<int, char>{}) == type_list<int, char>{});
static_assert(trim_front<1>(type_list<int, char>{}) == type_list<char>{});
static_assert(trim_front<2>(type_list<int, char>{}) == type_list<>{});
static_assert(trim_front<0>(type_list<int, char, float>{}) ==
              type_list<int, char, float>{});
static_assert(trim_front<1>(type_list<int, char, float>{}) ==
              type_list<char, float>{});
static_assert(trim_front<2>(type_list<int, char, float>{}) ==
              type_list<float>{});
static_assert(trim_front<3>(type_list<int, char, float>{}) == type_list<>{});

// Trim the back.
//
static_assert(trim_back<0>(type_list<>{}) == type_list<>{});
static_assert(trim_back<0>(type_list<int>{}) == type_list<int>{});
static_assert(trim_back<1>(type_list<int>{}) == type_list<>{});
static_assert(trim_back<0>(type_list<int, char>{}) == type_list<int, char>{});
static_assert(trim_back<1>(type_list<int, char>{}) == type_list<int>{});
static_assert(trim_back<2>(type_list<int, char>{}) == type_list<>{});
static_assert(trim_back<0>(type_list<int, char, float>{}) ==
              type_list<int, char, float>{});
static_assert(trim_back<1>(type_list<int, char, float>{}) ==
              type_list<int, char>{});
static_assert(trim_back<2>(type_list<int, char, float>{}) == type_list<int>{});
static_assert(trim_back<3>(type_list<int, char, float>{}) == type_list<>{});

// Get a subrange of types from a given 'type_list' instance.
//
static_assert(range<0, 0>(type_list<>{}) == type_list<>{});
static_assert(range<0, 0>(type_list<int>{}) == type_list<>{});
static_assert(range<1, 1>(type_list<int>{}) == type_list<>{});
static_assert(range<0, 1>(type_list<int>{}) == type_list<int>{});
static_assert(range<0, 0>(type_list<int, char>{}) == type_list<>{});
static_assert(range<0, 1>(type_list<int, char>{}) == type_list<int>{});
static_assert(range<0, 2>(type_list<int, char>{}) == type_list<int, char>{});
static_assert(range<1, 2>(type_list<int, char>{}) == type_list<char>{});
static_assert(range<2, 2>(type_list<int, char>{}) == type_list<>{});
static_assert(range<0, 0>(type_list<int, char, float>{}) == type_list<>{});
static_assert(range<0, 1>(type_list<int, char, float>{}) == type_list<int>{});
static_assert(range<0, 2>(type_list<int, char, float>{}) ==
              type_list<int, char>{});
static_assert(range<0, 3>(type_list<int, char, float>{}) ==
              type_list<int, char, float>{});
static_assert(range<0, 1>(type_list<int, char, float>{}) == type_list<int>{});
static_assert(range<1, 2>(type_list<int, char, float>{}) == type_list<char>{});
static_assert(range<2, 3>(type_list<int, char, float>{}) == type_list<float>{});
static_assert(range<0, 2>(type_list<int, char, float>{}) ==
              type_list<int, char>{});
static_assert(range<1, 3>(type_list<int, char, float>{}) ==
              type_list<char, float>{});

// Swap two elements.
//
static_assert(swap<0, 0>(type_list<int>{}) == type_list<int>{});
//
static_assert(swap<0, 0>(type_list<int, char>{}) == type_list<int, char>{});
static_assert(swap<0, 1>(type_list<int, char>{}) == type_list<char, int>{});
static_assert(swap<1, 0>(type_list<int, char>{}) == type_list<char, int>{});
static_assert(swap<1, 1>(type_list<int, char>{}) == type_list<int, char>{});
//
static_assert(swap<0, 0>(type_list<int, char, float>{}) ==
              type_list<int, char, float>{});
static_assert(swap<0, 1>(type_list<int, char, float>{}) ==
              type_list<char, int, float>{});
static_assert(swap<0, 2>(type_list<int, char, float>{}) ==
              type_list<float, char, int>{});
static_assert(swap<1, 0>(type_list<int, char, float>{}) ==
              type_list<char, int, float>{});
static_assert(swap<1, 1>(type_list<int, char, float>{}) ==
              type_list<int, char, float>{});
static_assert(swap<1, 2>(type_list<int, char, float>{}) ==
              type_list<int, float, char>{});
static_assert(swap<2, 0>(type_list<int, char, float>{}) ==
              type_list<float, char, int>{});
static_assert(swap<2, 1>(type_list<int, char, float>{}) ==
              type_list<int, float, char>{});
static_assert(swap<2, 2>(type_list<int, char, float>{}) ==
              type_list<int, char, float>{});

// Merge two sorted 'type_list' instances.
//
static_assert(merge(type_list<>{}, type_list<>{}, less) == type_list<>{});
//
static_assert(merge(type_list<>{}, type_list<int>{}, less) == type_list<int>{});
static_assert(merge(type_list<int>{}, type_list<>{}, less) == type_list<int>{});
static_assert(merge(type_list<>{}, type_list<char, int>{}, less) ==
              type_list<char, int>{});
static_assert(merge(type_list<char, int>{}, type_list<>{}, less) ==
              type_list<char, int>{});
//
static_assert(merge(type_list<char>{}, type_list<int>{}, less) ==
              type_list<char, int>{});
static_assert(merge(type_list<int>{}, type_list<char>{}, less) ==
              type_list<char, int>{});
//
static_assert(merge(type_list<char, short>{}, type_list<int>{}, less) ==
              type_list<char, short, int>{});
static_assert(merge(type_list<char>{}, type_list<short, int>{}, less) ==
              type_list<char, short, int>{});
//
static_assert(merge(type_list<char, short>{}, type_list<int, double>{}, less) ==
              type_list<char, short, int, double>{});
static_assert(merge(type_list<char, int>{}, type_list<short, double>{}, less) ==
              type_list<char, short, int, double>{});
static_assert(merge(type_list<char, double>{}, type_list<short, int>{}, less) ==
              type_list<char, short, int, double>{});
static_assert(merge(type_list<short, int>{}, type_list<char, double>{}, less) ==
              type_list<char, short, int, double>{});
//
static_assert(merge(type_list<char>{}, type_list<short, int, double>{}, less) ==
              type_list<char, short, int, double>{});
static_assert(merge(type_list<short>{}, type_list<char, int, double>{}, less) ==
              type_list<char, short, int, double>{});
static_assert(merge(type_list<int>{}, type_list<char, short, double>{}, less) ==
              type_list<char, short, int, double>{});
static_assert(merge(type_list<double>{}, type_list<char, short, int>{}, less) ==
              type_list<char, short, int, double>{});

// Sort a list of types.
//
static_assert(sort(type_list<>{}, less) == type_list<>{});
//
static_assert(sort(type_list<int>{}, less) == type_list<int>{});
static_assert(sort(type_list<char>{}, less) == type_list<char>{});
//
static_assert(sort(type_list<char, int>{}, less) == type_list<char, int>{});
static_assert(sort(type_list<int, char>{}, less) == type_list<char, int>{});
static_assert(sort(type_list<unsigned, int>{}, less) ==
              type_list<unsigned, int>{});
static_assert(sort(type_list<int, unsigned>{}, less) ==
              type_list<int, unsigned>{});
//
static_assert(sort(type_list<char, short, int>{}, less) ==
              type_list<char, short, int>{});
static_assert(sort(type_list<char, int, short>{}, less) ==
              type_list<char, short, int>{});
static_assert(sort(type_list<short, char, int>{}, less) ==
              type_list<char, short, int>{});
static_assert(sort(type_list<short, int, char>{}, less) ==
              type_list<char, short, int>{});
static_assert(sort(type_list<int, char, short>{}, less) ==
              type_list<char, short, int>{});
static_assert(sort(type_list<int, short, char>{}, less) ==
              type_list<char, short, int>{});
//
static_assert(sort(type_list<short, unsigned, char, int>{},
                   []<typename x, typename y> {
                     return sizeof(x) >= sizeof(y);
                   }) == type_list<unsigned, int, short, char>{});
static_assert(sort(type_list<short, unsigned, char, int>{},
                   []<typename x, typename y> {
                     return sizeof(x) > sizeof(y);
                   }) == type_list<int, unsigned, short, char>{});

// Transform
//
static_assert(transform(type_list<float, void, int>{}, []<typename x> {
                if constexpr (equal<x, void>)
                  return type_list<type_list<>>{};
                else
                  return type_list<x>{};
              }) == type_list<float, type_list<>, int>{});

namespace meta = xstd::meta;
using xstd::float32;

template <typename type>
constexpr auto string_from() {
  return typeid(type).name();
}
template <>
constexpr auto string_from<int>() {
  return "int";
}
template <>
constexpr auto string_from<float32>() {
  return "float32";
}
template <>
constexpr auto string_from<std::string>() {
  return "std::string";
}

// static_assert(transform(type_list<float, void, int>{}, []<typename x> {
//                 if constexpr (equal<x, void>)
//                   return type_list<type_list<>>{};
//                 else
//                   return type_list<x>{};
//               }) == type_list<float, type_list<>, int>{});

int main() {
  constexpr auto types = meta::type_list<int, float, std::string>{};

  int i = 0;
  for_each(types, [&]<typename type> {
    std::print("type {} = {}\n", i++, string_from<type>());
  });

  std::string str{};
  for_each(types, [&str]<typename type> {
    str += string_from<type>();
    str += std::format(", ");
  });
  std::print("types = {}\n", str);

  // meta::print_type(transform_and_fold(
  //     types, []<typename type> { return meta::value<type>; },
  //     [](auto... args) { return std::tuple<meta::unwrap<args>...>{}; }));

  // meta::print_type<meta::unwrap<transform<std::variant>(types)>>();
  // meta::print_type(transform(types, []<typename type> {
  //   if constexpr (meta::equal<type, int>)
  //     return meta::value<type>;
  //   else
  //     return meta::type_list<type>{};
  // }));
}
