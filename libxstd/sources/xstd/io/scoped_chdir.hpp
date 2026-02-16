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

/// RAII guard to temporarily change the current working directory (CWD)
/// It saves the CWD upon construction and restores it upon destruction.
///
/// WARNING:
/// This is NOT thread-safe. Unfortunately, the CWD is a process-wide global state.
/// If another thread changes the CWD or relies on it while this guard is active,
/// race conditions and undefined behavior in file resolution will occur.
/// Only use this in strictly single-threaded contexts or during startup/shutdown.
///
struct scoped_chdir final {
  explicit scoped_chdir(std::filesystem::path const& path)
      : cwd(std::filesystem::current_path()) {
    // Throws `std::filesystem::filesystem_error` if the path cannot be accessed.
    std::filesystem::current_path(path);
  }

  ~scoped_chdir() noexcept {
    // Use `std::error_code` overload to ensure no exceptions are thrown.
    // Otherwise, the `noexcept` would immediately lead to program termination.
    std::error_code ec;
    std::filesystem::current_path(cwd, ec);
    // If it fails here, there is little we can do:
    // Swallow the error and leave the CWD as it is.
  }

  // As RAII guard, do not allow copy or move operations.
  scoped_chdir(const scoped_chdir&) = delete;
  scoped_chdir& operator=(const scoped_chdir&) = delete;
  scoped_chdir(scoped_chdir&&) = delete;
  scoped_chdir& operator=(scoped_chdir&&) = delete;

 private:
  std::filesystem::path cwd;
};

}  // namespace xstd
