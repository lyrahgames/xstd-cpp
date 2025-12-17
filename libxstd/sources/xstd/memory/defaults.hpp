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
#include <xstd/memory/defaults.hpp>

namespace xstd {

/// With respect to the given offset,
/// compute the distance to the next offset
/// that fulfills the given alignment.
///
constexpr auto aligned_offset_padding(size_t offset, size_t alignment) noexcept
    -> size_t {
  return alignment - 1 - ((offset + alignment - 1) % alignment);
}

/// With respect to the given offset,
/// compute the next offset
/// that fulfills the given alignment.
///
constexpr auto aligned_offset(size_t offset, size_t alignment) noexcept
    -> size_t {
  return offset + aligned_offset_padding(offset, alignment);
}

}  // namespace xstd
