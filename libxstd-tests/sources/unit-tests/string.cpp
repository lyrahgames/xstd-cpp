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
#include <doctest/doctest.h>
//
#include <xstd/string.hpp>

using namespace std::literals;

SCENARIO("xstd::views::lines") {
  {
    std::string str{};
    for (auto line :
         xstd::views::lines("Hello\nWorld\nThis is\nC++20 coroutines"))
      str += std::format("> {};", line);
    CHECK(str == "> Hello;> World;> This is;> C++20 coroutines;");
  }
  {
    std::string str{};
    for (auto [i, line] : "Hello\nWorld\nThis is\nC++20 coroutines"sv |
                              xstd::views::lines | std::views::enumerate)
      str += std::format("{} {};", i, line);
    CHECK(str == "0 Hello;1 World;2 This is;3 C++20 coroutines;");
  }
}
