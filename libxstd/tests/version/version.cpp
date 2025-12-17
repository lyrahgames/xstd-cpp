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
#include <iomanip>
#include <iostream>
#include <sstream>
//
#include <xstd/version.hpp>

using namespace std;
using namespace xstd;

int main() {
  // All the tests will be run asynchronously.
  // This leads to interleaved output in the terminal.
  // To prevent this for the version output, everything is written
  // into a 'stringstream' that is printed at the end at once.
  //
  stringstream output;
  output << "\nxstd version:\n\n";

#define CAPTURE(X) \
  output << left << setw(20) << #X << " = " << right << setw(40) << X << '\n'

  CAPTURE(version::number);
  CAPTURE(version::string);
  CAPTURE(version::id);
  CAPTURE(version::full);
  CAPTURE(version::major);
  CAPTURE(version::minor);
  CAPTURE(version::patch);
  CAPTURE(version::snapshot_sn);
  CAPTURE(version::snapshot_id);

#undef CAPTURE

  cout << output.str() << endl;
}
