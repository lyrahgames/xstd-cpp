// Copyright © 2026 Markus Pawellek
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
import std;
import xstd;

using namespace xstd::meta;

template <string str, radix_node_instance... nodes>
using leaf = radix_node<str, true, nodes...>;
template <string str, radix_node_instance... nodes>
using node = radix_node<str, false, nodes...>;

static_assert(         //
    radix_tree<>{} ==  //
    radix_tree<node<"">>{});

// static_assert(radix_tree_from<>() == radix_tree<>{});

static_assert(                //
    radix_tree_from<"">() ==  //
    radix_tree<leaf<"">>{});

static_assert(                    //
    radix_tree_from<"help">() ==  //
    radix_tree<                   //
        node<"",                  //
             leaf<"help">>>{});

static_assert(                            //
    radix_tree_from<"help", "help">() ==  //
    radix_tree<                           //
        node<"",                          //
             leaf<"help">>>{});

static_assert(                        //
    radix_tree_from<"help", "">() ==  //
    radix_tree<                       //
        leaf<"",                      //
             leaf<"help">>>{});

static_assert(                            //
    radix_tree_from<"help", "helo">() ==  //
    radix_tree<                           //
        node<"",                          //
             node<"hel",                  //
                  leaf<"o">,              //
                  leaf<"p">>>>{});

static_assert(                               //
    radix_tree_from<"help", "version">() ==  //
    radix_tree<                              //
        node<"",                             //
             leaf<"help">,                   //
             leaf<"version">>>{});

static_assert(                           //
    radix_tree_from<"help", "hel">() ==  //
    radix_tree<                          //
        node<"",                         //
             leaf<"hel",                 //
                  leaf<"p">>>>{});

static_assert(  //
    radix_tree_from<"help",
                    "hello",
                    "version",
                    "verbose",
                    "very",
                    "input",
                    "in",
                    "out",
                    "output">() ==  //
    radix_tree<                     //
        node<"",                    //
             node<"hel",            //
                  leaf<"lo">,       //
                  leaf<"p">>,       //
             leaf<"in",             //
                  leaf<"put">>,     //
             leaf<"out",            //
                  leaf<"put">>,     //
             node<"ver",            //
                  leaf<"bose">,     //
                  leaf<"sion">,     //
                  leaf<"y">>>>{});

template <radix_node_instance node, string prefix>
constexpr void print() {
  constexpr auto str = prefix + '|' + label(node{});
  for_each(children(node{}), [&]<typename child> { print<child, str>(); });
  if constexpr (is_leaf(node{})) std::println("{}", str);
}

constexpr void print(radix_tree_instance auto tree) {
  for_each(children(root(tree)),
           [&]<typename child> { print<child, label(root(tree))>(); });
  if constexpr (is_leaf(root(tree))) std::println("{}", label(root(tree)));
}

inline void print_visit(radix_tree_instance auto tree, std::string_view str) {
  const auto visited = visit(tree, str, []<string key> {
    std::println("'{}' has been visited.", key);
  });
  if (!visited) std::println("'{}' not found.", str);
}

inline void print_traverse(radix_tree_instance auto tree,
                           std::string_view str) {
  const auto traversed =
      traverse(tree, str, [&]<string key>(std::string_view tail) {
        std::println("'{}' = '{}' + '{}'", str, key, tail);
      });
  if (!traversed) std::println("No prefix of '{}' found.", str);
}

int main() {
  constexpr auto tree =
      radix_tree_from<"help", "version", "helo", "hel", "verbose", "help-me",
                      "abc", "key", "check", "make", "input", "output", "man",
                      "cheat", "in", "out", "help", "help">();

  // breakpoint(tree);

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
