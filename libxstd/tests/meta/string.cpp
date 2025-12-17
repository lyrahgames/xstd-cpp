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
#include <xstd/meta/string.hpp>

using namespace std;
using namespace xstd::meta;

static_assert(std::ranges::contiguous_range<decltype(""_xs)>);
static_assert(std::ranges::contiguous_range<decltype("help"_xs)>);
static_assert(std::ranges::contiguous_range<decltype("with space"_xs)>);

static_assert(substring<2, 4>("--help"_xs) == "help"_xs);
static_assert(prefix<2>("--help"_xs) == "--"_xs);
static_assert(tail<2>("--help"_xs) == "help"_xs);
static_assert(suffix<3>("my_file.txt"_xs) == "txt"_xs);
static_assert(prefix_match_index("version"_xs, "verbose"_xs) == 3);
static_assert(prefix_match_index("version"_xs, ""_xs) == 0);
static_assert(prefix_match_index("help"_xs, "help"_xs) == 4);

// static_assert(string_instance<"help"_xs>);
// static_assert(!string_instance<1>);

// static_assert(is_string("help"_xs));
// static_assert(!is_string("help"));
// static_assert(!is_string(1.0f));
// static_assert(!is_string('a'));

// void check() {
//   for (auto c : "abc"_xs) print("c = {}", c);
//   const auto v = static_cast<std::string_view>("xyz"_xs);
// }

int main() {
  assert(std::format("{}", "xyz"_xs) == "xyz");
  assert(static_cast<std::string_view>("abc"_xs) == "abc");
}
