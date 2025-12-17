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
namespace xstd::meta::detail::radix_tree {

// This is the implementation of the type predicate that checks
// whether a given type is an instance of the 'node' template.
//
template <typename T>
struct is_node_implementation : std::false_type {};
//
template <string str, node_list_instance children, bool is_leaf>
struct is_node_implementation<node<str, children, is_leaf>> : std::true_type {};

// To establish a total order for nodes based on their strings,
// we compare the first characters of the strings, ensuring uniqueness
// when new strings are inserted into the static radix tree.
//
constexpr auto node_order = []<node_instance p, node_instance q> {
  return p::prefix[0] <= q::prefix[0];
};

// The actual implementation of the insertion routine is done recursively.
// For this, five cases have to be taken into account concerning
// the given string, the node string, and their prefix match index.
//
// 1. No Match:
//    Both strings do not match and the index is zero.
// 2. Full Match:
//    Both strings are the same and the prefix match index is their length.
// 3. Node Match:
//    The node string is fully matched as a prefix by the string.
//    The prefix match index will be the size of the node string.
// 4. String Match:
//    The string is fully matched as a prefix by the node string.
//    The prefix match index will be the size of the string.
// 5. Partial Match:
//    Both strings are matched by a non-empty prefix and
//    the prefix match index will be smaller than both string sizes.
//
// Thereby, we assume that the empty string is used for the root node
// and that this one matches every string.
// In all other cases, a prefix match index of zero means no match at all.
//
// For the implementation of the basic insertion mechanism,
// it is convenient to provide an additional implementation template
// for which the prefix match index has already been calculated.
// This is 'basic_insert_implementation'.
//
// Case: No Match
//
// In this case, the insertion should not change the given node at all.
//
template <string str, size_t index>
consteval auto basic_insert_implementation(node_instance auto root)
  requires(!prefix(root).empty()) && (index == 0)
{
  return root;
}
//
// Case: Full Match
//
// In this case, the path and node already exist inside the tree.
// The node may not be marked as a leaf.
// So, we have to make it a leaf and leave the rest as it is.
// The full match is therefore a projection and
// makes sure that a given string can only be inserted once.
//
template <string str, size_t index>
consteval auto basic_insert_implementation(node_instance auto root)
  requires(index == prefix(root).size()) && (index == str.size())
{
  return leaf_assign(root);
}
//
// Case: String Match
//
// In this case, the current node needs to be split.
// The new node will be a leaf with the prefix as its string.
// The old node with its children will be the child of the new node.
// Hereby, the string of the old node will be changed to the tail.
//
template <string str, size_t index>
consteval auto basic_insert_implementation(node_instance auto root)
  requires(index < prefix(root).size()) && (index == str.size())
{
  constexpr auto p = str;
  constexpr auto q = tail<index>(prefix(root));
  return leaf_from<p>(node_list_from(prefix_assign<q>(root)));
}
//
// Case: Partial Match
//
// In this case, the current node needs to be split.
// It will provide two children.
// One with its former children and the tail of the node string.
// The other node will provide no children and the tail of the string.
// The new node itself is no leaf.
//
// At this point, the order of the insertion is important,
// if lexicographical order in the radix tree is of interest.
//
template <string str, size_t index>
consteval auto basic_insert_implementation(node_instance auto root)
  requires(index > 0) && (index < prefix(root).size()) && (index < str.size())
{
  constexpr auto p = prefix<index>(str);
  constexpr auto q1 = tail<index>(prefix(root));
  constexpr auto q2 = tail<index>(str);

  constexpr auto children1 = node_list_from(prefix_assign<q1>(root));
  constexpr auto children2 = insert<leaf<q2>>(children1, node_order);

  return node_from<p>(children2);
}
//
// Checking if any of the node children matches a prefix of a given string
// can be simply done by checking the first character
// in addition with some syntax for variadic templates.
//
template <string str, node_instance... nodes>
consteval bool match_exists(node_list<nodes...>) {
  return ((str[0] == nodes::prefix[0]) || ...);
}
//
// If no children matches any prefix,
// the given string will be inserted as a new leaf with no children
// into the current children list of the given node.
//
// At this point, the order of the insertion is important,
// if lexicographical order in the radix tree is of interest.
//
template <string str, size_t index, bool matched>
  requires(!matched)
consteval auto node_match(node_instance auto root) {
  constexpr auto p = prefix(root);
  constexpr auto q = tail<index>(str);

  constexpr auto new_children = insert<leaf<q>>(children(root), node_order);
  return node_from<p, is_leaf(root)>(new_children);
}
//
// When there is a child of the current node
// which matches a prefix of the given string
// then by construction of the radix tree
// this node has to be unique.
// In such a case, we syntactically insert the given string into all children
// by recursively calling the basic insertion function
// together with the type list transformation algorithm,
// knowing that all children that do not match any prefix
// will not be changed by this transformation.
//
template <string str, size_t index, bool matched>
  requires(matched)
consteval auto node_match(node_instance auto root) {
  constexpr auto p = prefix(root);
  constexpr auto q = tail<index>(str);

  constexpr auto new_children = transform(children(root), [&]<node_instance n> {
    return node_list_from(basic_insert<q>(n{}));
  });
  return node_from<p, is_leaf(root)>(new_children);
}
//
// Case: Node Match
//
// This case is a little bit more complex.
// Because the node was fully matched,
// the given string tail needs to be forwarded to its children.
// If no children matches a prefix with the tail,
// a new node has to be inserted into the children of the current node.
// In the other case, we can finally recursively call the basic insertion.
//
template <string str, size_t index>
consteval auto basic_insert_implementation(node_instance auto root)
  requires(index == prefix(root).size()) && (index < str.size())
{
  constexpr auto q = tail<index>(str);
  constexpr bool matched = match_exists<q>(children(root));
  return node_match<str, index, matched>(root);
}

///
///
template <string str>
consteval auto basic_insert(node_instance auto root) {
  constexpr auto index = prefix_match_index(prefix(root), str);
  return basic_insert_implementation<str, index>(root);
}

///
///
template <string... str>
  requires(sizeof...(str) == 0)
consteval auto insert_implementation(node_instance auto root) {
  return root;
}
//
template <string str, string... tail>
consteval auto insert_implementation(node_instance auto root) {
  return insert_implementation<tail...>(basic_insert<str>(root));
}

// The 'prefix_match' function attempts to match a static prefix,
// defined as a template parameter, against a dynamic string.
// Compiler optimization, including 'constexpr' inlining,
// ensures direct code generation without function calls or loops.
// If no match is found, 'nullptr' is returned;
// otherwise, the tail of the dynamic string is returned.
//
// It serves as a private helper function, primarily employed
// in the implementation of the 'visit' and 'traverse' algorithms.
//
template <string prefix, size_t index = 0>
constexpr auto prefix_match(czstring str) noexcept -> czstring {
  if constexpr (index == prefix.size())
    return str;
  else {
    if (prefix[index] != *str++) return nullptr;
    return prefix_match<prefix, index + 1>(str);
  }
}

template <string prefix = "">
constexpr bool visit_implementation(node_instance auto r,
                                    czstring str,
                                    auto&& f) {
  using root = decltype(r);

  constexpr auto new_prefix = prefix + root::prefix;
  const auto tail = prefix_match<root::prefix>(str);
  if (!tail) return false;
  if constexpr (root::is_leaf) {
    if (!*tail) {
      std::forward<decltype(f)>(f).template operator()<new_prefix>();
      return true;
    }
  } else {
    if (!*tail) return false;
  }
  return for_each_until(typename root::children{}, [&]<node_instance child> {
    return visit_implementation<new_prefix>(child{}, tail, f);
  });
}

template <string prefix = "">
constexpr bool traverse_implementation(node_instance auto r,
                                       czstring str,
                                       auto&& f) {
  using root = decltype(r);

  const auto tail = prefix_match<root::prefix>(str);
  if (tail) {
    constexpr auto new_prefix = prefix + root::prefix;
    const auto found =
        for_each_until(typename root::children{}, [&]<node_instance child> {
          return traverse_implementation<new_prefix>(child{}, tail, f);
        });
    if constexpr (root::is_leaf) {
      if (!found)
        std::forward<decltype(f)>(f).template operator()<new_prefix>(tail);
      return true;
    } else
      return found;
  } else
    return false;
}

}  // namespace xstd::meta::detail::radix_tree
