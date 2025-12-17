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

/// The 'type_list' template is a template meta programming utility.
/// Within a 'type_list', packaging, accessing, and modifying types
/// originally given by variadic template parameters is possible.
/// All 'type_list' instances are empty and can be used as tag types.
///
template <typename... types>
struct type_list {};

// For tight constraints in template requirements,
// we want to be able to decide whether a given type is
// an instance of the 'type_list' template.
//
// The implementation is inside the 'detail' namespace and
// carried out by a struct template predicate specialization.
//
namespace detail {

template <typename type>
struct is_type_list : std::false_type {};

template <typename... types>
struct is_type_list<type_list<types...>> : std::true_type {};

}  // namespace detail

/// This is a concept that checks whether a given type
/// is an instance of the 'type_list' template.
///
template <typename type>
concept type_list_instance = detail::is_type_list<type>::value;

///
/// Equality and Inequality
///

// /// Check whether two instances of 'type_list' are the same.
// ///
// template <typename... x, typename... y>
// consteval auto operator==(type_list<x...>, type_list<y...>) {
//   return false;
// }
// //
// template <typename... x>
// consteval auto operator==(type_list<x...>, type_list<x...>) {
//   return true;
// }

// /// Check whether two instances of 'type_list' are not the same.
// ///
// template <typename... x, typename... y>
// consteval auto operator!=(type_list<x...> a, type_list<y...> b) {
//   return !(a == b);
// }

///
/// Accessors and Predicates
///
/// Retrieve information from a given `type_list`
/// instance without changing its content.
/// Predicates can be viewed as accessors that return a boolean value.
///

/// Return the size of a given 'type_list' instance.
///
template <typename... types>
consteval auto size(type_list<types...>) -> size_t {
  return sizeof...(types);
}

/// Check whether a given 'type_list' instance contains nothing.
///
consteval auto empty(type_list_instance auto list) {
  return size(list) == 0;
}

/// Access a specific element of a 'type_list' instance by its index.
/// The function returns a contained type as value by using `type_wrapper`
///
template <size_t index>
consteval auto element(type_list<>) = delete;
//
template <size_t index, typename type, typename... types>
  requires(index == 0)
consteval auto element(type_list<type, types...>) {
  return as_value<type>;
}
//
template <size_t index, typename type, typename... types>
consteval auto element(type_list<type, types...> list)
  requires((0 < index) && (index < size(list)))
{
  return element<index - 1>(type_list<types...>{});
}

/// Access the first element of a 'type_list' instance.
/// The returned type is represented by a `type_wrapper` instance.
/// This function leads to compile errors
/// if the given 'type_list' instance is empty.
///
consteval auto front(type_list_instance auto list)
  requires(!empty(list))
{
  return element<0>(list);
}
//
consteval auto operator*(type_list_instance auto list) {
  return type_list<as_type<front(list)>>{};
}

/// Access the last element of a 'type_list' instance.
/// The returned type is represented by a `type_wrapper` instance.
/// This function leads to compile errors
/// if the given 'type_list' instance is empty.
///
template <type_list_instance list>
consteval auto back(list x)
  requires(!empty(x))
{
  return element<size(list{}) - 1>(x);
}

namespace detail {

template <typename functor, typename type>
concept type_predicate_for = requires(functor f) {
  { f.template operator()<type>() } -> std::same_as<bool>;
};

template <typename functor, typename... types>
consteval bool type_predicate_for_type_list(type_list<types...>) {
  return (type_predicate_for<functor, types> && ...);
}

}  // namespace detail

/// Check whether a given functor can be applied as type predicate
/// to all given types returning `true` or `false`.
///
template <typename functor, typename... types>
concept type_predicate_for =
    (detail::type_predicate_for<functor, types> && ...);

/// Check whether a given functor can be applied as type predicate
/// to all given types contained in a given `type_list` instance.
///
template <typename functor, typename list>
concept type_predicate_for_type_list =
    type_list_instance<list> &&
    detail::type_predicate_for_type_list<functor>(list{});

/// Check whether a condition provided by a type predicate holds
/// for all types inside the given 'type_list' instance.
///
template <typename... types>
consteval auto all_of(type_list<types...>,
                      type_predicate_for<types...> auto&& f) {
  return (f.template operator()<types>() && ...);
}

/// Check whether a condition provided by a type predicate holds
/// for at least one type inside the given 'type_list' instance.
///
template <typename... types>
consteval auto any_of(type_list<types...>,
                      type_predicate_for<types...> auto&& f) {
  return (f.template operator()<types>() || ...);
}

/// Check whether a condition provided by a type predicate holds
/// for none of the types inside the given `type_list` instance.
///
template <typename... types>
consteval auto none_of(type_list<types...>,
                       type_predicate_for<types...> auto&& f) {
  return (!f.template operator()<types>() && ...);
}
// template <type_list_instance list, type_list_type_predicate<list> functor>
// consteval auto none_of(list types, functor&& f) {
//   return !any_of(types, std::forward<decltype(f)>(f));
// }

/// Check whether a given type is contained
/// inside a given 'type_list' instance.
///
template <typename type>
consteval auto contained(type_list_instance auto list) {
  return any_of(list, []<typename t> { return std::same_as<t, type>; });
}

/// Check whether a given 'type_list' slice
/// is contained inside a 'type_list' instance.
///
template <typename type>
consteval auto contained(type_list_instance auto list, type_list<type>) {
  return contained<type>(list);
}

/// Check whether a given `type_list` instance may represent
/// a mathematical set that contains each element only once.
///
consteval bool elementwise_unique(type_list<>) {
  return true;
}
//
template <typename type, typename... types>
consteval bool elementwise_unique(type_list<type, types...>) {
  constexpr auto tail = type_list<types...>{};
  return !contained<type>(tail) && elementwise_unique(tail);
}

///
/// Modifiers
///

/// Concatenate all types of two given 'type_list' instances
/// and wrap them again as a `type_list` instance.
///
template <typename... x, typename... y>
consteval auto concat(type_list<x...>, type_list<y...>) {
  return type_list<x..., y...>{};
}
//
consteval auto operator+(type_list_instance auto x, type_list_instance auto y) {
  return concat(x, y);
}

/// Returns a new instance of 'type_list' by adding
/// the given type to the front of the given 'type_list' instance.
///
template <typename type, typename... types>
consteval auto push_front(type_list<types...>) {
  return type_list<type, types...>{};
}

/// Returns a new instance of 'type_list' by adding
/// the given type to the back of the given 'type_list' instance.
///
template <typename type, typename... types>
consteval auto push_back(type_list<types...>) {
  return type_list<types..., type>{};
}

/// Remove the first element of a 'type_list' instance.
///
consteval auto pop_front(type_list<>) = delete;
//
template <typename type, typename... types>
consteval auto pop_front(type_list<type, types...>) {
  return type_list<types...>{};
}
//
consteval auto operator--(type_list_instance auto list) {
  return pop_front(list);
}

/// Remove the last element of a 'type_list' instance.
///
consteval auto pop_back(type_list<>) = delete;
//
template <typename type>
consteval auto pop_back(type_list<type>) {
  return type_list<>{};
}
//
consteval auto pop_back(type_list_instance auto list) {
  return *list + pop_back(--list);
}
//
consteval auto operator--(type_list_instance auto list, int) {
  return pop_back(list);
}

/// Reverse the order of types inside a 'type_list' instance.
///
consteval auto reverse(type_list<> list) {
  return list;
}
//
consteval auto reverse(type_list_instance auto list) {
  return reverse(--list) + *list;
}
//
consteval auto operator~(type_list_instance auto list) {
  return reverse(list);
}

/// Remove a given amount of types from the front of a 'type_list' instance.
///
template <size_t n>
  requires(n == 0)
consteval auto trim_front(type_list_instance auto list) {
  return list;
}
//
template <size_t n>
consteval auto trim_front(type_list_instance auto list)
  requires((0 < n) && (n <= size(list)))
{
  return trim_front<n - 1>(pop_front(list));
}

/// Remove a given amount of types from the back of a 'type_list' instance.
///
template <size_t n>
  requires(n == 0)
consteval auto trim_back(type_list_instance auto list) {
  return list;
}
//
template <size_t n>
consteval auto trim_back(type_list_instance auto list)
  requires((0 < n) && (n <= size(list)))
{
  return trim_back<n - 1>(pop_back(list));
}

/// Get a subrange of types from a 'type_list' instance.
///
template <size_t first, size_t last>
consteval auto range(type_list_instance auto list)
  requires((first <= last) && (last <= size(list)))
{
  constexpr auto n = size(list);
  return trim_front<first>(trim_back<n - last>(list));
}

/// Insert a type at a given index into a 'type_list' instance;
///
template <size_t index, typename type>
consteval auto insert(type_list_instance auto list)
  requires(index <= size(list))
{
  if constexpr (index == 0)
    return push_front<type>(list);
  else
    // return push_front<decltype(front(list))>(
    //     insert<index - 1, type>(pop_front(list)));
    return *list + insert<index - 1, type>(--list);
}

/// Insert a type into a 'type_list' instance by using a predicate.
///
template <typename type>
consteval auto insert(type_list<>, auto less) {
  return type_list<type>{};
}
//
template <typename type>
consteval auto insert(type_list_instance auto list, auto less) {
  if constexpr (less.template operator()<type, as_type<front(list)>>())
    return push_front<type>(list);
  else
    return *list + insert<type>(--list, less);
}

/// Return the index of a type inside a `type_list` instance.
/// To successfully compile, the list needs to represent a set
/// and must contain the given type.
/// In such a way, the given list will not be empty and
/// it will always be possible to find the specified element.
///
template <typename type>
consteval auto index(type_list_instance auto list) -> size_t
  requires(elementwise_unique(list)) && (contained<type>(list))
{
  if constexpr (meta::equal<type, as_type<front(list)>>)
    return 0;
  else
    return 1 + index<type>(--list);
}

/// Return the index to the first type inside a `type_list`
/// instance that fulfills the given predicate.
/// To successfully compile, the list must contain at least one such type.
///
template <type_list_instance list>
consteval auto index(list, type_predicate_for_type_list<list> auto f) -> size_t
  requires(any_of(list{}, f))
{
  if constexpr (f.template operator()<as_type<front(list{})>>())
    return 0;
  else
    return 1 + index(--list{}, f);
}

/// Remove a type at a given index from a 'type_list' instance.
///
template <size_t index>
consteval auto remove(type_list_instance auto list)
  requires(index < size(list))
{
  if constexpr (index == 0)
    return --list;
  else
    return *list + remove<index - 1>(--list);
}

/// Remove all types from a 'type_list' instance
/// for which the given predicate returns 'true'.
///
consteval auto remove_all(type_list<> list, auto f) {
  return list;
}
//
consteval auto remove_all(type_list_instance auto list, auto f) {
  if constexpr (f.template operator()<as_type<front(list)>>())
    return remove_all(--list, f);
  else
    return *list + remove_all(--list, f);
}

/// Swap two types given by their position inside a 'type_list' instance.
///
template <size_t i, size_t j>
consteval auto swap(type_list_instance auto list)
  requires((i == j) && (j < size(list)))
{
  return list;
}
//
template <size_t i, size_t j>
consteval auto swap(type_list_instance auto list)
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
consteval auto swap(type_list_instance auto list)
  requires((j < i) && (i < size(list)))
{
  return swap<j, i>(list);
}

/// Merge two sorted 'type_list' instances by using a 'less' predicate.
///
consteval auto merge(type_list_instance auto left,
                     type_list_instance auto right,
                     auto less)
  requires(empty(left) || empty(right))
{
  return left + right;
}
//
consteval auto merge(type_list_instance auto left,
                     type_list_instance auto right,
                     auto less) {
  if constexpr (less.template
                operator()<as_type<front(left)>, as_type<front(right)>>())
    return *left + merge(--left, right, less);
  else
    return *right + merge(left, --right, less);
}

/// Sort a 'type_list' instance by using a 'less' predicate.
///
consteval auto sort(type_list_instance auto list, auto less) {
  constexpr auto n = size(list);
  constexpr auto half = n / 2;
  return merge(sort(range<0, half>(list), less),
               sort(range<half, n>(list), less), less);
}
//
consteval auto sort(type_list_instance auto list, auto less)
  requires(size(list) < 2)
{
  return list;
}

/// Check whether a given functor can be used
/// as a type transformer that returns a `type_list` instance
/// that wraps other types for each contained element
/// inside a given `type_list` instance.
///
namespace detail {
template <typename functor, typename type>
concept type_transformer_for = requires(functor f) {
  { f.template operator()<type>() } -> type_list_instance;
};
}  // namespace detail
template <typename functor, typename... types>
concept type_transformer_for =
    (detail::type_transformer_for<functor, types> && ...);

/// Apply a given type transformer functor to each element of a given `type_list`
/// instance and return a `type_list` instance that contains the modified types.
/// Modified types that are returned from the functor return types
/// by wrapping them in a `type_list` instance.
/// This also allows to return no types or more than one type
/// for each contained type, leading to the capability to add and remove elements.
///
template <typename... types>
consteval auto transform(type_list<types...>,
                         type_transformer_for<types...> auto f) {
  return (f.template operator()<types>() + ...);
}

///
/// Algorithms for Code Generation
///

/// Check whether a given functor can be used as a type folder
/// for all contained elements at once inside a given `type_list` instance.
///
template <typename functor, typename... types>
concept type_folder_for = requires(functor f) {
  { f.template operator()<types...>() } -> not_void;
};

/// Apply a type folder to all contained elements
/// inside a given `type_list` instance at once.
///
template <typename... types>
constexpr auto fold(type_list<types...>, type_folder_for<types...> auto&& f) {
  return f.template operator()<types...>();
}

/// Check whether a given functor can be invoked for each given type.
///
namespace detail {
template <typename functor, typename type>
concept type_functor_for =
    requires(functor f) { f.template operator()<type>(); };
}  // namespace detail
template <typename functor, typename... types>
concept type_functor_for = (detail::type_functor_for<functor, types> && ...);

/// Invoke a given type functor for each type
/// contained in the `type_list` instance.
///
template <typename... types>
constexpr void for_each(type_list<types...>,
                        type_functor_for<types...> auto&& f) {
  (f.template operator()<types>(), ...);
}

/// Check whether a given functor can be invoked for each given type
/// of a variadic template parameter list or inside a given `type_list` instance.
///
namespace detail {
template <typename functor, typename type>
concept bool_type_functor_for = requires(functor f) {
  { f.template operator()<type>() } -> std::same_as<bool>;
};
//
template <typename functor, typename... types>
consteval bool bool_type_functor_for_type_list(type_list<types...>) {
  return (bool_type_functor_for<functor, types> && ...);
}
}  // namespace detail
//
template <typename functor, typename... types>
concept bool_type_functor_for =
    (detail::bool_type_functor_for<functor, types> && ...);
//
template <typename functor, typename list>
concept bool_type_functor_for_type_list =
    type_list_instance<list> &&  //
    detail::bool_type_functor_for_type_list<functor>(list{});

/// Apply a functor subsequently to each element of a
/// given `type_list` instance until it returns `true`.
///
template <type_list_instance types,
          bool_type_functor_for_type_list<types> functor>
constexpr bool for_each_until(types list, functor&& f) {
  if constexpr (empty(list))
    return false;
  else {
    if (f.template operator()<as_type<front(list)>>()) return true;
    return for_each_until(--list, std::forward<functor>(f));
  }
}

}  // namespace xstd::meta
