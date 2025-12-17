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
#include <xstd/io.hpp>

using namespace xstd;

namespace {

void string_from_file_test(std::filesystem::path const& path,
                           std::string_view str) {
  if (exists(path))
    throw std::runtime_error(std::format(
        "Path '{}' exists at the start of the test.", path.string()));
  {  // Create file with content.
    std::ofstream file{path, std::ios::binary};
    file << str;
  }
  const auto result = string_from_file(path);
  remove(path);  // Delete file after reading for clean-up.
  REQUIRE(result);
  REQUIRE(result.value() == str);
}

}  // namespace

SCENARIO("xstd::string_from_file") {
  CHECK(not string_from_file("invalid"));
  string_from_file_test("empty.txt", "");
  string_from_file_test("test.txt", "Hello, World!");
  string_from_file_test("test.txt", R"##(
  ## Copyright and License

  The copyright for the code is held by the contributors of the code.
  The revision history in the version control system is the primary source of authorship information for copyright purposes.
  Please see individual source files for appropriate copyright notices.
  For more information, see the file [`AUTHORS.md`](AUTHORS.md).

  `xstd` is free software, distributed under the terms of the GNU General
  Public License as published by the Free Software Foundation,
  version 3 of the License (or any later version).  For more information,
  see the [GNU General Public License][GPLv3] or the file [`COPYING.md`](COPYING.md).

  `xstd` is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  Copyright years on `xstd` source files may be listed using range notation, e.g., 1987-2012, indicating that every year in the range, inclusive, is a copyrightable year that could otherwise be listed individually.

  Copying and distribution of this file, with or without modification, are permitted in any medium without royalty provided the copyright notice and this notice are preserved.
  This file is offered as-is, without any warranty.
  )##");
}
