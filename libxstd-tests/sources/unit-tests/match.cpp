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
#include <xstd/functional/match.hpp>
#include <xstd/functional/variant.hpp>
#include <xstd/math.hpp>

using namespace xstd;

namespace {

template <std::floating_point real>
auto sqrt(real x) -> std::variant<real, std::invalid_argument> {
  if (x < 0) return std::invalid_argument{"negative number"};
  return std::sqrt(x);
}

auto test(float x) {
  return sqrt(x) | match{
                       [](float x) { return x; },
                       [](std::invalid_argument) { return -1.0f; },
                   };
}

}  // namespace

SCENARIO("Exception-Free Error Handling with std::variant and match") {
  CHECK(test(-10.0f) == -1.0f);
  CHECK(test(1.0f) == 1.0f);
}
