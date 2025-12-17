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
#include <xstd/meta/tag.hpp>

namespace xstd::meta {

/// The template 'value_list' is a template meta programming utility.
/// Within a 'value_list' instance, packaging, accessing, and modifying
/// constexpr values originally given by variadic template parameters is possible.
/// All instances of `value_list` are empty and can be used as tag types.
///
template <auto... values>
struct value_list {};

// For tight constraints in template requirements,
// we want to be able to decide whether a given type is
// an instance of the 'value_list' template.
//
// The implementation is taken inside the 'details' namespace
// by a typical struct template predicate specialization.
//
namespace detail {

template <typename type>
struct is_value_list : std::false_type {};

template <auto... values>
struct is_value_list<value_list<values...>> : std::true_type {};

}  // namespace detail

/// Check whether a given type is an instance of 'value_list'.
///
template <typename type>
concept value_list_instance = detail::is_value_list<type>::value;

///
/// Equality and Inequality
///

// /// Check whether two instances of 'value_list' are the same.
// ///
// consteval auto operator==(value_list_instance auto x,
//                           value_list_instance auto y) {
//   return false;
// }
// //
// template <value_list_instance list>
// consteval auto operator==(list, list) {
//   return true;
// }

// /// Check whether two instances of 'value_list' are not equal.
// ///
// consteval auto operator!=(value_list_instance auto x,
//                           value_list_instance auto y) {
//   return !(x == y);
// }

///
/// Accessors and Predicates
///
/// Retrieve information from a given `value_list` instance
/// without changing its content.
/// Predicates are accessors that return a boolean value.
///

/// Return the size of a given 'value_list' instance.
///
template <auto... values>
consteval auto size(value_list<values...>) -> size_t {
  return sizeof...(values);
}

/// Check whether a given 'value_list' instance contains nothing.
///
consteval auto empty(value_list_instance auto list) {
  return size(list) == 0;
}

/// Access a specific value of a 'value_list' instance by its index.
///
template <size_t index>
consteval auto element(value_list<>) = delete;
//
template <size_t index, auto value, auto... values>
consteval auto element(value_list<value, values...>)
  requires(index == 0)
{
  return value;
}
//
template <size_t index, auto value, auto... values>
consteval auto element(value_list<value, values...> list)
  requires((0 < index) && (index < size(list)))
{
  return element<index - 1>(value_list<values...>{});
}

/// Access the first element of a 'value_list' instance.
///
consteval auto front(value_list_instance auto list)
  requires(!empty(list))
{
  return element<0>(list);
}

/// Receive the first element of the given `value_list` instance
/// wrapped inside `value_list` to allow for further processing.
///
consteval auto operator*(value_list_instance auto list) {
  return value_list<front(list)>{};
}

/// Access the last element of a 'value_list' instance.
///
consteval auto back(value_list_instance auto list)
  requires(!empty(list))
{
  return element<size(list) - 1>(list);
}

namespace detail {

template <typename functor, auto value>
concept value_predicate_for = requires(functor f) {
  { f.template operator()<value>() } -> std::same_as<bool>;
};

template <typename functor, auto... values>
consteval bool value_predicate_for_value_list(value_list<values...>) {
  return (value_predicate_for<functor, values> && ...);
}

}  // namespace detail

/// Check whether a given functor can be applied as predicate
/// to all given values returning `true` or `false`.
///
template <typename functor, auto... values>
concept value_predicate_for =
    (detail::value_predicate_for<functor, values> && ...);

/// Check whether a given functor can be applied as predicate
/// to all elements contained in a given `value_list` instance.
///
template <typename functor, typename list>
concept value_predicate_for_value_list =
    value_list_instance<list> &&
    detail::value_predicate_for_value_list<functor>(list{});

/// Check whether a condition provided by a value predicate holds
/// for all elements inside the given 'value_list' instance.
///
template <auto... values>
consteval auto all_of(value_list<values...>,
                      value_predicate_for<values...> auto&& f) {
  return (f.template operator()<values>() && ...);
}

/// Check whether a condition provided by a value predicate holds
/// for at least one element inside the given 'value_list' instance.
///
template <auto... values>
consteval auto any_of(value_list<values...>,
                      value_predicate_for<values...> auto&& f) {
  return (f.template operator()<values>() || ...);
}

/// Check whether a condition provided by a value predicate holds
/// for none of the elements inside the given `value_list` instance.
///
template <auto... values>
consteval auto none_of(value_list<values...>,
                       value_predicate_for<values...> auto&& f) {
  return (!f.template operator()<values>() && ...);
}

/// Check whether a given value is contained
/// inside a given 'value_list' instance.
///
template <auto value>
consteval bool contained(value_list_instance auto list) {
  return any_of(list, []<auto x> { return strict_equal(x, value); });
}
//
consteval auto contained(value_list_instance auto list, auto value) {
  return any_of(list, [value]<auto x> { return strict_equal(x, value); });
}

/// Check whether a given `value_list` instance may represent
/// a mathematical set that contains each element only once.
///
consteval bool elementwise_unique(value_list<>) {
  return true;
}
//
template <auto x, auto... values>
consteval bool elementwise_unique(value_list<x, values...>) {
  constexpr auto tail = value_list<values...>{};
  return !contained<x>(tail) && elementwise_unique(tail);
}

///
/// Modifiers
///

/// Concatenate all elements of two given 'value_list' instances
/// and wrap them again as a `value_list` instance.
///
template <auto... x, auto... y>
consteval auto concat(value_list<x...>, value_list<y...>) {
  return value_list<x..., y...>{};
}
//
consteval auto operator+(value_list_instance auto x,
                         value_list_instance auto y) {
  return concat(x, y);
}

/// Return a new instance of 'value_list' by prepending
/// the given value to the given 'value_list' instance.
///
template <auto value, auto... values>
consteval auto push_front(value_list<values...>) {
  return value_list<value, values...>{};
}

/// Return a new instance of 'value_list' by appending
/// the given value to the given 'value_list' instance.
///
template <auto value, auto... values>
consteval auto push_back(value_list<values...>) {
  return value_list<values..., value>{};
}

/// Remove the first element of a 'value_list' instance.
///
consteval auto pop_front(value_list<>) = delete;
//
template <auto value, auto... values>
consteval auto pop_front(value_list<value, values...>) {
  return value_list<values...>{};
}
//
consteval auto operator--(value_list_instance auto list) {
  return pop_front(list);
}

/// Remove the last element of a 'value_list' instance.
///
consteval auto pop_back(value_list<>) = delete;
//
template <auto value>
consteval auto pop_back(value_list<value>) {
  return value_list<>{};
}
//
consteval auto pop_back(value_list_instance auto list) {
  return *list + pop_back(--list);
}
//
consteval auto operator--(value_list_instance auto list, int) {
  return pop_back(list);
}

/// Reverse the order of types inside a 'value_list' instance.
///
consteval auto reverse(value_list<> list) {
  return list;
}
//
consteval auto reverse(value_list_instance auto list) {
  return reverse(--list) + *list;
}
//
consteval auto operator~(value_list_instance auto list) {
  return reverse(list);
}

/// Remove a given amount of elements from
/// the front of a 'value_list' instance.
///
template <size_t n>
  requires(n == 0)
consteval auto trim_front(value_list_instance auto list) {
  return list;
}
//
template <size_t n>
consteval auto trim_front(value_list_instance auto list)
  requires((0 < n) && (n <= size(list)))
{
  return trim_front<n - 1>(pop_front(list));
}

/// Remove a given amount of elements from
/// the back of a 'value_list' instance.
///
template <size_t n>
  requires(n == 0)
consteval auto trim_back(value_list_instance auto list) {
  return list;
}
//
template <size_t n>
consteval auto trim_back(value_list_instance auto list)
  requires((0 < n) && (n <= size(list)))
{
  return trim_back<n - 1>(pop_back(list));
}

/// Get a subrange of values from a 'value_list' instance.
///
template <size_t first, size_t last>
consteval auto range(value_list_instance auto list)
  requires((first <= last) && (last <= size(list)))
{
  constexpr auto n = size(list);
  return trim_front<first>(trim_back<n - last>(list));
}

/// Insert a value at a given index into a 'value_list' instance.
///
template <size_t index, auto value>
consteval auto insert(value_list_instance auto list)
  requires(index <= size(list))
{
  if constexpr (index == 0)
    return push_front<value>(list);
  else
    return *list + insert<index - 1, value>(--list);
}

/// Insert a value into a 'value_list' instance by using a predicate.
///
template <auto value>
consteval auto insert(value_list<>, auto less) {
  return value_list<value>{};
}
//
template <auto value>
consteval auto insert(value_list_instance auto list, auto less) {
  if constexpr (less.template operator()<value, front(list)>())
    return push_front<value>(list);
  else
    return *list + insert<value>(--list, less);
}

/// Return the index of an element inside a `value_list` instance.
/// To successfully compile, the list needs to represent a set
/// and must contain the given value.
/// In such a way, the given list will not be empty and
/// it will always be possible to find the specified element.
///
template <auto value>
consteval auto index(value_list_instance auto list) -> size_t
  requires(elementwise_unique(list)) && (contained<value>(list))
{
  if constexpr (strict_equal(value, front(list)))
    return 0;
  else
    return 1 + index<value>(--list);
}

/// Remove an element at a given index from a 'value_list' instance.
///
template <size_t index>
consteval auto remove(value_list_instance auto list)
  requires(index < size(list))
{
  if constexpr (index == 0)
    return --list;
  else
    return *list + remove<index - 1>(--list);
}

/// Remove all elements from a 'value_list' instance
/// for which the given predicate returns 'true'.
///
consteval auto remove_all(value_list<> list, auto f) {
  return list;
}
//
consteval auto remove_all(value_list_instance auto list, auto f) {
  if constexpr (f.template operator()<front(list)>())
    return remove_all(--list, f);
  else
    return *list + remove_all(--list, f);
}

/// Swap two values given by their position inside a 'value_list' instance.
///
template <size_t i, size_t j>
consteval auto swap(value_list_instance auto list)
  requires((i == j) && (j < size(list)))
{
  return list;
}
//
template <size_t i, size_t j>
consteval auto swap(value_list_instance auto list)
  requires((i < j) && (j < size(list)))
{
  constexpr auto n = size(list);
  return range<0, i>(list) +      //
         range<j, j + 1>(list) +  //
         range<i + 1, j>(list) +  //
         range<i, i + 1>(list) +  //
         range<j + 1, n>(list);
}
//
template <size_t i, size_t j>
consteval auto swap(value_list_instance auto list)
  requires((j < i) && (i < size(list)))
{
  return swap<j, i>(list);
}

/// Merge two sorted 'value_list' instances by using a 'less' predicate.
///
consteval auto merge(value_list_instance auto left,
                     value_list_instance auto right,
                     auto less)
  requires(empty(left) || empty(right))
{
  return left + right;
}
//
consteval auto merge(value_list_instance auto left,
                     value_list_instance auto right,
                     auto less) {
  if constexpr (less.template operator()<front(left), front(right)>())
    return *left + merge(--left, right, less);
  else
    return *right + merge(left, --right, less);
}

/// Sort a 'value_list' instance by using a 'less' predicate.
///
consteval auto sort(value_list_instance auto list, auto less) {
  constexpr auto n = size(list);
  constexpr auto half = n / 2;
  return merge(sort(range<0, half>(list), less),
               sort(range<half, n>(list), less), less);
}
//
consteval auto sort(value_list_instance auto list, auto less)
  requires(size(list) < 2)
{
  return list;
}

/// Check whether a given functor can be used
/// as a value transformer that returns a `type_list` instance
/// that wraps other types for each contained element
/// inside a given `type_list` instance.
///
namespace detail {
template <typename functor, auto value>
concept value_transformer_for = requires(functor f) {
  { f.template operator()<value>() } -> not_void;
};
}  // namespace detail
template <typename functor, auto... values>
concept value_transformer_for =
    (detail::value_transformer_for<functor, values> && ...);

/// Apply a given value transformer functor to each element
/// of a given `value_list` instance and return a `value_list` instance
/// that contains the modified values.
///
template <auto... values>
consteval auto transform(value_list<values...>,
                         value_transformer_for<values...> auto f) {
  return value_list<f.template operator()<values>()...> {};
}
consteval auto transform(value_list<>, auto f) {
  return value_list<>{};
}

///
/// Algorithms for Code Generation
///

/// Check whether a given functor can be used as a value folder
/// for all contained elements at once inside a given `value_list` instance.
///
template <typename functor, auto... values>
concept value_folder_for = requires(functor f) {
  { f.template operator()<values...>() } -> not_void;
};

/// Apply a value folder to all contained elements
/// inside a given `value_list` instance at once.
///
template <auto... values>
constexpr auto fold(value_list<values...>,
                    value_folder_for<values...> auto&& f) {
  return f.template operator()<values...>();
}

/// Check whether a given functor can be invoked for each given value.
///
namespace detail {
template <typename functor, auto value>
concept value_functor_for =
    requires(functor f) { f.template operator()<value>(); };
}  // namespace detail
template <typename functor, auto... values>
concept value_functor_for = (detail::value_functor_for<functor, values> && ...);

/// Invoke a given value functor for each element
/// contained in the `value_list` instance.
///
template <auto... values>
constexpr void for_each(value_list<values...>,
                        value_functor_for<values...> auto&& f) {
  (f.template operator()<values>(), ...);
}

/// Check whether a given functor can be invoked for each given element
/// of a variadic template parameter list or inside a given `value_list` instance.
///
namespace detail {
template <typename functor, auto value>
concept bool_value_functor_for = requires(functor f) {
  { f.template operator()<value>() } -> std::same_as<bool>;
};
//
template <typename functor, auto... values>
consteval bool bool_value_functor_for_value_list(value_list<values...>) {
  return (bool_value_functor_for<functor, values> && ...);
}
}  // namespace detail
//
template <typename functor, auto... values>
concept bool_value_functor_for =
    (detail::bool_value_functor_for<functor, values> && ...);
//
template <typename functor, typename list>
concept bool_value_functor_for_value_list =
    value_list_instance<list> &&  //
    detail::bool_value_functor_for_value_list<functor>(list{});

/// Apply a functor subsequently to each element of a
/// given `value_list` instance until it returns `true`.
///
template <value_list_instance values,
          bool_value_functor_for_value_list<values> functor>
constexpr bool for_each_until(values list, functor&& f) {
  if constexpr (empty(list))
    return false;
  else {
    if (f.template operator()<front(list)>()) return true;
    return for_each_until(--list, std::forward<functor>(f));
  }
}

}  // namespace xstd::meta
