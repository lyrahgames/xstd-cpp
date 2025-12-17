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
#include <xstd/meta/detail/radix_tree/node.hpp>
#include <xstd/meta/string_list.hpp>

namespace xstd::meta {

/// Instances of the 'radix_tree' template
/// are tag types that represent a radix tree.
/// Each instance provides access to the root of the tree.
/// It must be constructed at compile time
/// and cannot change during runtime.
/// However, by using algorithms 'visit' and 'traverse',
/// it is possible to check at runtime whether a dynamic string
/// is fully contained or a prefix matches inside the radix tree.
///
template <detail::radix_tree::node_instance node = detail::radix_tree::node<"">>
struct radix_tree {
  using root = node;

  constexpr radix_tree() noexcept = default;

  // This constructor allows for CTAD.
  //
  constexpr radix_tree(root) noexcept {}
};

namespace detail {

template <typename type>
struct is_radix_tree : std::false_type {};

template <typename type>
struct is_radix_tree<meta::radix_tree<type>> : std::true_type {};

}  // namespace detail

/// Checks whether the given type is an instance of `radix_tree`.
///
template <typename type>
concept radix_tree_instance = detail::is_radix_tree<type>::value;

///
/// Constructor Extensions
///

/// Construct a `radix_tree` instance from
/// strings given as template arguments.
///
template <string... str>
consteval auto radix_tree_from() {
  return insert<str...>(radix_tree<>{});
}

/// Construct a `radix_tree` instance from
/// a given `string_list` instance.
///
template <string... str>
consteval auto radix_tree_from(string_list<str...>) {
  return radix_tree_from<str...>();
}

///
/// Equality and Inequality
///

// /// Check whether two instances of 'radix_tree' are the same.
// ///
// consteval auto operator==(radix_tree_instance auto x,
//                           radix_tree_instance auto y) {
//   return false;
// }
// //
// template <radix_tree_instance tree>
// consteval auto operator==(tree, tree) {
//   return true;
// }

// /// Check whether two instances of 'radix_tree' are not the same.
// ///
// consteval auto operator!=(radix_tree_instance auto x,
//                           radix_tree_instance auto y) {
//   return !(x == y);
// }

///
/// Accessors
///

/// Return the root node of the given `radix_tree` instance.
///
consteval auto root(radix_tree_instance auto tree) {
  return typename decltype(tree)::root{};
}

///
/// Modifiers
///

/// Insert a variable number of strings into a given instance
/// of `radix_tree` and return the newly constructed instance.
///
template <string... str>
consteval auto insert(radix_tree_instance auto tree) {
  return radix_tree{insert<str...>(root(tree))};
}

///
/// Algorithms
///

/// The visit algorithm tries to match the whole string
/// with a static string contained inside the radix tree.
/// If the given string is not contained in the tree,
/// the algorithm returns false and does not call the function object.
/// In the other case, the algorithm returns true
/// and calls the function object
/// with the static string provided as template parameter.
///
constexpr auto visit(radix_tree_instance auto tree, czstring str, auto&& f) {
  return visit(root(tree), str, std::forward<decltype(f)>(f));
}

/// The traverse algorithm tries to match the longest prefix
/// of the given string contained in the given radix tree.
/// If no prefix can be matched the algorithm returns `false`
/// and does not call the function object.
/// If a prefix can be matched, the function object is called
/// with the static prefix and the dynamic tail.
///
constexpr auto traverse(radix_tree_instance auto tree, czstring str, auto&& f) {
  return traverse(root(tree), str, std::forward<decltype(f)>(f));
}

}  // namespace xstd::meta
