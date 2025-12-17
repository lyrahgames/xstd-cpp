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
#include <xstd/meta/string.hpp>
#include <xstd/meta/type_list.hpp>

namespace xstd::meta::detail::radix_tree {

// A tree is typically defined as a recursive data structure.
// So, we would like to offer recursive constraints and concepts.
// For this, we pre-declare the template predicate which decides
// whether a given type is a node of a static radix tree.
//
template <typename type>
struct is_node_implementation;

/// This type function checks whether the given
/// type is an instance of the 'node' template.
///
template <typename type>
constexpr bool is_node = is_node_implementation<type>::value;

/// This type function checks whether a given type
/// is an instance of 'type_list' and all contained types
/// are instances of the 'node' template.
///
template <typename list>
constexpr bool is_node_list =
    type_list_instance<list> &&
    all_of(list{}, []<typename type> { return is_node<type>; });

/// This concept checks whether a given type is an instance of 'node'.
///
template <typename T>
concept node_instance = is_node<T>;

/// This concept checks whether a given type is a 'type_list' instance
/// such that all contained types are instances of the 'node' template.
///
template <typename list>
concept node_list_instance = is_node_list<list>;

/// A node list is a 'type_list' instance that
/// contains only instances of the 'node' template.
///
template <node_instance... nodes>
using node_list = type_list<nodes...>;

/// The actual 'node' template can be defined
/// with correct and recursive constraints.
///
template <string str, node_list_instance nodes = node_list<>, bool leaf = false>
struct node {
  static constexpr string prefix = str;
  using children = nodes;
  static constexpr bool is_leaf = leaf;
};

/// 'leaf' is an alias to the 'node' template.
/// It is used to simplify marking nodes as leaves.
///
template <string str, node_list_instance children = node_list<>>
using leaf = node<str, children, true>;

///
/// Constructor Extensions
///

///
///
template <string str, bool is_leaf = false>
consteval auto node_from(node_list_instance auto nodes) {
  return node<str, decltype(nodes), is_leaf>{};
}

///
///
template <string str>
consteval auto leaf_from(node_list_instance auto nodes) {
  return node_from<str, true>(nodes);
}

///
///
template <node_instance... nodes>
consteval auto node_list_from(nodes...) {
  return node_list<nodes...>{};
}

///
/// Ordering
///

/// Check whether two instances of 'node' are the same.
///
consteval auto operator==(node_instance auto x, node_instance auto y) {
  return false;
}
//
template <node_instance list>
consteval auto operator==(list, list) {
  return true;
}

/// Check whether two instances of 'node' are not the same.
///
consteval auto operator!=(node_instance auto x, node_instance auto y) {
  return !(x == y);
}

///
/// Accessors
///

///
///
consteval auto prefix(node_instance auto root) {
  return decltype(root)::prefix;
}

///
///
consteval auto children(node_instance auto root) {
  return typename decltype(root)::children{};
}

///
///
consteval auto is_leaf(node_instance auto root) {
  return decltype(root)::is_leaf;
}

///
/// Modifiers
///

/// This function takes a 'node' instance as input
/// and returns a new 'node' instance with the same children
/// and leaf property but with a newly set string.
///
template <string str>
consteval auto prefix_assign(node_instance auto root) {
  return node_from<str, is_leaf(root)>(children(root));
}

///
///
consteval auto leaf_assign(node_instance auto root) {
  return leaf_from<prefix(root)>(children(root));
}

///
///
// consteval auto children_assign(node_instance auto root) {
//   return node_from<>();
// }

///
///
template <string... str>
consteval auto insert(node_instance auto root) {
  return insert_implementation<str...>(root);
}

///
/// Algorithms
///

/// The visit algorithm tries to match the whole string
/// with a static string contained inside the static radix tree.
/// If the given string is not contained in the tree,
/// the algorithm returns false and does not call the function object.
/// In the other case, the algorithm returns true
/// and calls the function object
/// with the static string provided as template parameter.
///
constexpr bool visit(node_instance auto root, czstring str, auto&& f) {
  return visit_implementation(root, str, std::forward<decltype(f)>(f));
}

/// The traverse algorithm tries to match the longest prefix
/// of the given string contained in the given static radix tree.
/// If no prefix can be matched the algorithm returns false
/// and does not call the function object.
/// If a prefix can be matched, the function object is called
/// with the static prefix and the dynamic tail.
///
constexpr bool traverse(node_instance auto root, czstring str, auto&& f) {
  return traverse_implementation(root, str, std::forward<decltype(f)>(f));
}

}  // namespace xstd::meta::detail::radix_tree

#include <xstd/meta/detail/radix_tree/node.ipp>
