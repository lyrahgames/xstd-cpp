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
#include <xstd/container.hpp>
#include <xstd/string.hpp>

using namespace xstd;

namespace {

// String-Likes
//
static_assert(string_like<string>);
static_assert(string_like<string_view>);
//
static_assert(string_like<std::string>);
static_assert(string_like<std::string_view>);
static_assert(string_like<std::vector<char>>);
static_assert(string_like<std::array<char, 10>>);
//
static_assert(not string_like<zstring>);
static_assert(not string_like<czstring>);
static_assert(not string_like<std::set<char>>);
static_assert(not string_like<std::list<char>>);
static_assert(not string_like<std::pair<char, char>>);
static_assert(not string_like<std::tuple<char, char, char>>);

// String Ranges
//
static_assert(stable_string_range<std::vector<std::string>>);
static_assert(stable_string_range<std::vector<std::string_view>>);
static_assert(stable_string_range<std::list<std::string>>);
static_assert(stable_string_range<std::set<std::string>>);
//
static_assert(string_range<std::generator<std::string>>);
static_assert(not stable_string_range<std::generator<std::string>>);
static_assert(stable_string_range<std::generator<std::string const&>>);
static_assert(stable_string_range<std::generator<std::string_view>>);
//
using transform_view_test =
    decltype(std::declval<std::vector<int>&>() |
             std::views::transform([](int i) { return std::to_string(i); }));
static_assert(string_range<transform_view_test>);
static_assert(not stable_string_range<transform_view_test>);

}  // namespace
