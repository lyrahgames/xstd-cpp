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
#include <xstd/io/defaults.hpp>

namespace xstd {

/// Reads the entire content of a file and returns it as a `std::string`.
/// This function does not throw and failures in opening or reading the
/// file, or determining its size, are reported via an empty optional.
/// Memory allocation failures result in program termination.
///
inline auto string_from_file(std::filesystem::path const& path) noexcept
    -> std::optional<std::string> {
  // Open the file in binary mode and read all characters as block.
  // Make sure to jump to its end for directly reading its size.
  std::ifstream file{path, std::ios::binary | std::ios::ate};
  if (!file) return {};
  const auto size = file.tellg();
  if (size < 0) return {};
  std::string result(size, '\0');
  file.seekg(0);  // Go back to the start.
  if (not file.read(result.data(), size)) return {};
  return result;
};

}  // namespace xstd
