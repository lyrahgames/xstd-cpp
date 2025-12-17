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
#include <iomanip>
#include <iostream>
#include <print>
//
#include <xstd/meta/radix_tree.hpp>

using xstd::meta::radix_tree;
using xstd::meta::radix_tree_from;

namespace detail = xstd::meta::detail;
using detail::radix_tree::leaf;
using detail::radix_tree::node;
using detail::radix_tree::node_list;

static_assert(radix_tree<>{} == radix_tree<node<"">>{});
static_assert(radix_tree_from<>() == radix_tree<>{});

static_assert(radix_tree_from<"">() == radix_tree<leaf<"">>{});

static_assert(radix_tree_from<"help">() == radix_tree<  //
                                               node<"",
                                                    node_list<  //
                                                        leaf<"help">>>>{});

static_assert(radix_tree_from<"help", "help">() ==
              radix_tree<  //
                  node<"",
                       node_list<  //
                           leaf<"help">>>>{});

static_assert(radix_tree_from<"help", "">() == radix_tree<  //
                                                   leaf<"",
                                                        node_list<  //
                                                            leaf<"help">>>>{});

static_assert(radix_tree_from<"help", "helo">() ==  //
              radix_tree<                           //
                  node<"",
                       node_list<  //
                           node<"hel",
                                node_list<      //
                                    leaf<"o">,  //
                                    leaf<"p">>>>>>{});

static_assert(radix_tree_from<"help", "version">() ==  //
              radix_tree<                              //
                  node<"",
                       node_list<         //
                           leaf<"help">,  //
                           leaf<"version">>>>{});

static_assert(radix_tree_from<"help", "hel">() ==  //
              radix_tree<                          //
                  node<"",
                       node_list<  //
                           leaf<"hel",
                                node_list<  //
                                    leaf<"p">>>>>>{});

static_assert(radix_tree_from<"help",
                              "hello",
                              "version",
                              "verbose",
                              "very",
                              "in",
                              "input",
                              "out",
                              "output">() ==  //
              radix_tree<node<"",
                              node_list<  //
                                  node<"hel",
                                       node_list<        //
                                           leaf<"lo">,   //
                                           leaf<"p">>>,  //
                                  leaf<"in",
                                       node_list<          //
                                           leaf<"put">>>,  //
                                  leaf<"out",
                                       node_list<          //
                                           leaf<"put">>>,  //
                                  node<"ver",
                                       node_list<         //
                                           leaf<"bose">,  //
                                           leaf<"sion">,  //
                                           leaf<"y">>>    //
                                  >>>{});

using xstd::czstring;
using namespace xstd;
using namespace xstd::meta;
using namespace std;

template <meta::detail::radix_tree::node_instance root, meta::string prefix>
constexpr void print() {
  constexpr auto str = prefix + '|' + root::prefix;
  for_each(typename root::children{},
           [&]<typename child> { print<child, str>(); });
  if constexpr (root::is_leaf) cout << '"' << str << '"' << endl;
}

constexpr void print(radix_tree_instance auto t) {
  using tree = decltype(t);
  for_each(children(root(tree{})),
           [&]<typename child> { print<child, tree::root::prefix>(); });
  if constexpr (tree::root::is_leaf)
    cout << '"' << tree::root::prefix << '"' << endl;
}

inline void print_visit(radix_tree_instance auto tree, czstring cstr) {
  const auto visited = visit(tree, cstr, []<meta::string str> {
    cout << '"' << str << '"' << " has been visited!" << endl;
  });
  if (!visited)
    cout << '"' << cstr << '"' << " is not inside the static radix tree."
         << endl;
}

inline void print_traverse(radix_tree_instance auto tree, czstring cstr) {
  const auto traversed =
      traverse(tree, cstr, [&]<meta::string str>(czstring tail) {
        cout << '"' << cstr << '"' << " visited the prefix \"" << str
             << "\" with the tail \"" << tail << "\"!" << endl;
      });
  if (!traversed)
    cout << '"' << cstr << '"'
         << " has no known prefix inside the static radix tree." << endl;
}

int main() {
  using meta::detail::radix_tree::leaf;
  using meta::detail::radix_tree::node;
  using meta::detail::radix_tree::node_list;

  // meta::print_type(
  //     static_radix_tree_from<"help", "hello", "version", "verbose", "very",
  //                            "in", "input", "out", "output">());

  constexpr auto tree =
      radix_tree_from<"help", "version", "helo", "hel", "verbose", "help-me",
                      "abc", "key", "check", "make", "input", "output", "man",
                      "cheat", "in", "out", "help", "help">();

  print(tree);

  print_visit(tree, "check");
  print_visit(tree, "help");
  print_visit(tree, "mine");
  print_visit(tree, "long");
  print_visit(tree, "verbose");

  print_traverse(tree, "check");
  print_traverse(tree, "help");
  print_traverse(tree, "mine");
  print_traverse(tree, "long");
  print_traverse(tree, "verbose");
  print_traverse(tree, "hela");
  print_traverse(tree, "key=uiae");
  print_traverse(tree, "xyz");
}
