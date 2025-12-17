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
#include <xstd/meta/defaults.hpp>
#include <xstd/string/defaults.hpp>

namespace xstd::meta {

// Static zero-terminated strings can be used as template arguments.
// C++20 support has to be enabled.

// We will not use a variant such as
// 'basic_static_zstring' to support 'wchar_t'.
// This would make things complicated and
// difficult to read without much benefit.

// The length has to be known.
//
template <size_t N>
  requires(N > 0)  // String must at least store a zero byte.
struct string {
  using iterator = zstring;
  using const_iterator = czstring;

  constexpr string() noexcept = default;

  // Enable implicit construction of static strings from C strings.
  //
  constexpr string(const char (&str)[N]) noexcept {
    for (size_t i = 0; i < N; ++i) _data[i] = str[i];
  }

  // Implicit Conversion to `std::string_view`
  //
  constexpr operator std::string_view() const noexcept {
    return {data(), size()};
  }

  /// Get index-based access to the characters.
  ///
  constexpr auto operator[](size_t index) noexcept -> char& {
    return _data[index];
  }
  constexpr auto operator[](size_t index) const noexcept -> char {
    return _data[index];
  }

  // The compiler needs to be able to compare the content of strings.
  //
  friend constexpr auto operator<=>(const string&,
                                    const string&) noexcept = default;

  /// Returns the size of the string without the terminating null character.
  ///
  static constexpr auto size() noexcept { return N - 1; }

  /// Checks whether the string is empty.
  ///
  static constexpr bool empty() noexcept { return size() == 0; }

  /// Get access to the raw underlying data.
  ///
  constexpr auto data() noexcept -> zstring { return _data; }
  constexpr auto data() const noexcept -> czstring { return _data; }

  /// Get acces to the underlying data by using iterators.
  ///
  constexpr auto begin() noexcept -> iterator { return &_data[0]; }
  constexpr auto begin() const noexcept -> const_iterator { return _data; }
  constexpr auto end() noexcept -> iterator { return _data + (N - 1); }
  constexpr auto end() const noexcept -> const_iterator {
    return _data + (N - 1);
  }

  // `string` must be a structural type to make it usable in template parameters.
  // Thus, it cannot declare its data as private.
  // private:
  char _data[N]{};
};

namespace detail {

template <typename type>
struct is_string : std::false_type {};

template <size_t n>
struct is_string<string<n>> : std::true_type {};

}  // namespace detail

/// Check whether a given type is an instance of `meta::string`.
///
template <typename type>
concept string_instance = detail::is_string<type>::value;

// Provide support for custom literal '_xs'.
//
template <string str>
constexpr auto operator""_xs() noexcept {
  return str;
}

// Append characters to static string should be compile-time enabled.
//
template <size_t N>
constexpr auto operator+(const string<N>& str, char c) noexcept {
  string<N + 1> result{};
  for (size_t i = 0; i < N - 1; ++i) result[i] = str[i];
  result[N - 1] = c;
  return result;
}

// Appending two static strings should also be compile-time enabled.
//
template <size_t N, size_t M>
constexpr auto operator+(const string<N>& str1,
                         const string<M>& str2) noexcept {
  string<N + M - 1> result{};
  size_t i = 0;
  for (; i < N - 1; ++i) result[i] = str1[i];
  for (; i < N + M - 1; ++i) result[i] = str2[i - N + 1];
  return result;
}

/// Returns a static string containing the substring
/// of the given string specified by offset and size.
///
template <size_t offset, size_t size, size_t N>
constexpr auto substring(string<N> str) noexcept  //
  requires(offset + size < N)
{
  string<size + 1> result{};
  for (size_t i = 0; i < size; ++i) result[i] = str[offset + i];
  return result;
}

/// Returns a static string containing the prefix
/// of the given string with given size.
///
template <size_t size, size_t N>
constexpr auto prefix(string<N> str) noexcept  //
  requires(size < N)
{
  return substring<0, size>(str);
}

/// Returns a static string containing the suffix
/// of the given string with given size.
///
template <size_t size, size_t N>
constexpr auto suffix(string<N> str) noexcept  //
  requires(size < N)
{
  return substring<N - 1 - size, size>(str);
}

/// Returns the first index at which the characters
/// of the two given strings are not equal.
///
template <size_t N, size_t M>
constexpr auto prefix_match_index(string<N> str1, string<M> str2) noexcept
    -> size_t {
  constexpr auto bound = ((N < M) ? N : M) - 1;
  for (size_t i = 0; i < bound; ++i)
    if (str1[i] != str2[i]) return i;
  return bound;
}

///
///
template <size_t index, size_t N>
constexpr auto tail(string<N> str) noexcept {
  return suffix<str.size() - index>(str);
}

/// `meta::string` Output Streamer for Printing via `cout`
///
template <size_t n>
inline auto operator<<(std::ostream& os, string<n> str) -> std::ostream& {
  return os << static_cast<std::string_view>(str);
}

}  // namespace xstd::meta

/// `meta::string` Formatter for `std::format` and `std::print`
///
template <size_t n>
struct std::formatter<xstd::meta::string<n>, char> {
  template <class context>
  constexpr auto parse(context& ctx) -> context::iterator {
    return ctx.begin();
  }
  template <class context>
  auto format(xstd::meta::string<n> str, context& ctx) const
      -> context::iterator {
    return ranges::copy(static_cast<std::string_view>(str), ctx.out()).out;
  }
};
