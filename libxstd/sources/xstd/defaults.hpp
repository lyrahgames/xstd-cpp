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

// Include Version Testing Header
//
#ifndef __has_include
#define __has_include(x) 0
#endif
#if __has_include(<version>)
#include <version>
#endif

// Headers for Error Handling
//
#include <cassert>
#include <stdexcept>
#include <system_error>

// Standard Type Definitions
//
#include <cstddef>
#include <cstdint>
#include <stdfloat>

// Standard Utilities
//
#include <utility>

namespace xstd {

using namespace std::literals;

// Standard Integer Type Aliases
//
using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;
//
using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

// Standard Floating-Point Type Aliases
// Should be `std::float32_t` and `std::float64_t`.
// However, the C++ standard does not ensure that
// these types behave in the same way as `float` and `double`.
//
using float32 = float;
using float64 = double;

/// Rust-Like Builtin Type Aliases
///
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
//
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
//
using isize = std::ptrdiff_t;
using usize = std::size_t;
//
using f16 = std::float16_t;
using f32 = std::float32_t;
using f64 = std::float64_t;
using f128 = std::float128_t;

}  // namespace xstd
