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
#include <xstd/defaults.hpp>
//
#include <bit>
#include <cmath>
#include <complex>
#include <limits>
#if __has_include(<linalg>)
#include <linalg>
#endif
#include <numbers>
#include <numeric>
#include <random>

namespace xstd {

// Standard Type for Floating-Point Computations
//
using real = float32;

// Provide quick standard math constants for standard floating-point type.
//
inline constexpr auto pi = std::numbers::pi_v<real>;
inline constexpr auto infinity = std::numeric_limits<real>::infinity();

}  // namespace xstd
