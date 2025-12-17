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
#include <xstd/ranges.hpp>
//
#include <cstring>
#include <format>
#include <regex>
#include <sstream>
#include <string>
#include <string_view>

namespace xstd {

using std::string;
using std::string_view;

// According to the C++ Core Guidelines,
// the usage of C-style strings should be clear
// by using appropriate type definitions.
// In the future, this might be handled by
// using the Guideline Support Library (GSL).
//
using zstring = char*;
using czstring = const char*;

// Defines a minimal string-like type as a contiguous range of characters.
// It is intentionally lightweight and does not enforce a full string interface.
// Also, `czstring` and `zstring` do not fulfill this concept - this is intentional.
//
// Notes:
// - This concept should not be used in interfaces that accept a single string.
//   Instead, use `std::string_view` or `std::string const&` to reduce complexity.
// - It is mainly used to define string ranges for function templates.
//
template <typename type, typename char_type>
concept basic_string_like =
    std::ranges::contiguous_range<type> &&
    std::same_as<std::decay_t<std::ranges::range_value_t<type>>, char_type>;

// Standard Specialization of `basic_string_like` for `char`
// Provides a convenient shorthand for most string-like algorithms.
//
// Notes:
//   - Conceptually similar to `std::string` specialization for `std::basic_string`
//
template <typename type>
concept string_like = basic_string_like<type, char>;

// A string range is a range whose elements are themselves string-like.
// That is, each element of the outer range satisfies the `string_like` concept.
// Use it to accept multiple lines or multiple strings in a single interface,
// e.g., for logging, parsing, or formatting functions.
//
// The outer range must satisfy `std::ranges::input_range`.
// As such, it only guarantees single-pass iteration:
//   - You can iterate over the elements only once.
//   - You cannot assume random access or repeated access to elements.
//   - Pointers or references may become invalid after moving the iterator.
//
template <typename type>
concept string_range = std::ranges::input_range<type> &&
                       string_like<std::ranges::range_value_t<type>>;

// Extends the `string_range` concept with additional guarantees
// about lifetime and stability of its elements.
// Either the inner string type is a borrowed range, that does not own the data,
// or the reference type of the range is an actual reference.
// This avoids temporaries being returned by lazily evaluated view.
// Useful for algorithms that require the strings to remain valid during iteration.
//
template <typename type>
concept stable_string_range =
    string_range<type> &&
    (std::ranges::borrowed_range<std::ranges::range_value_t<type>> ||
     std::is_lvalue_reference_v<std::ranges::range_reference_t<type>>);

}  // namespace xstd
