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
#include <xstd/io/string_from_file.hpp>

namespace xstd::fdm {

using address = std::filesystem::path;

constexpr auto message_address(address addr) {
  return addr += ".fdm";
}

constexpr auto send_address(address addr) {
  return addr += ".send";
}

constexpr auto recv_address(address addr) {
  return addr += ".recv";
}

inline void send(address const& domain, std::string_view msg) {
  const auto msg_addr = message_address(domain);
  const auto tmp_addr = send_address(msg_addr);
  {
    std::ofstream file{tmp_addr, std::ios::binary};
    file << msg;
  }
  rename(tmp_addr, msg_addr);
}

inline auto recv(address const& domain) -> std::optional<std::string> {
  const auto msg_addr = message_address(domain);
  const auto tmp_addr = recv_address(msg_addr);

  std::error_code ec;
  rename(msg_addr, tmp_addr, ec);
  if (ec) return {};

  auto msg = string_from_file(tmp_addr);
  remove(tmp_addr);
  // return std::move(msg);  // Move because NRVO cannot be applied.
  return msg;  // NRVO works because `string_from_file` returns same type.
}

}  // namespace xstd::fdm
