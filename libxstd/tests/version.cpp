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
import std;
import xstd;

using namespace std;

int main() {
  // All the tests will be run asynchronously.
  // This leads to interleaved output in the terminal.
  // To prevent this for the version output, everything is written
  // into a 'stringstream' that is printed at the end at once.
  //
  stringstream output;
  output << "\nxstd version:\n\n";

#define CAPTURE(X) \
  output << left << setw(30) << #X << " = " << right << setw(40) << X << '\n'

  CAPTURE(xstd::version::number);
  CAPTURE(xstd::version::string);
  CAPTURE(xstd::version::id);
  CAPTURE(xstd::version::full);
  CAPTURE(xstd::version::major);
  CAPTURE(xstd::version::minor);
  CAPTURE(xstd::version::patch);
  CAPTURE(xstd::version::snapshot_sn);
  CAPTURE(xstd::version::snapshot_id);

#undef CAPTURE

  cout << output.str() << endl;
}
