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
#include <version>

import std;
import xstd;

using namespace xstd;

// String-Likes
static_assert(string_like<std::string>);
static_assert(string_like<std::string_view>);
static_assert(string_like<std::vector<char>>);
static_assert(string_like<std::array<char, 10>>);
//
static_assert(not string_like<char*>);
static_assert(not string_like<const char*>);
static_assert(not string_like<std::set<char>>);
static_assert(not string_like<std::list<char>>);
static_assert(not string_like<std::pair<char, char>>);
static_assert(not string_like<std::tuple<char, char, char>>);

// String Ranges
static_assert(stable_string_range<std::vector<std::string>>);
static_assert(stable_string_range<std::vector<std::string_view>>);
static_assert(stable_string_range<std::list<std::string>>);
static_assert(stable_string_range<std::set<std::string>>);
//
#if __cpp_lib_generator >= 202207L
static_assert(string_range<std::generator<std::string>>);
static_assert(not stable_string_range<std::generator<std::string>>);
static_assert(stable_string_range<std::generator<std::string const&>>);
static_assert(stable_string_range<std::generator<std::string_view>>);
#endif

int main() {
  std::vector<int> data{123, 456, 59, 7, 9};
  auto strs =
      std::views::transform(data, [](int n) { return std::to_string(n); });
  static_assert(string_range<decltype(strs)>);
  static_assert(not stable_string_range<decltype(strs)>);
  print(strs);
}
