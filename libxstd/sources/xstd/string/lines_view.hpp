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

namespace xstd {

/// A lightweight, constexpr-capable view that lazily iterates over the lines
/// of a `std::string_view`.
///
/// This view performs zero allocations and returns `std::string_view` elements
/// referring directly to the underlying source. It is suitable when the input
/// is already stored and stable in memory.
///
/// A `lines_view` does not know its size and does not provide random access.
/// It is a single-pass, forward-only range. For storing the lines or performing
/// indexed access, collect the results into a container.
///
/// Splitting is performed on the newline character `'\n'`. Trailing carriage
/// returns `'\r'` (as in CRLF sequences) are trimmed from each line.
///
struct string_lines_view : std::ranges::view_interface<string_lines_view> {
  struct iterator {
    using iterator_concept = std::forward_iterator_tag;
    using iterator_category = std::input_iterator_tag;
    using value_type = std::string_view;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type const*;
    using reference = value_type;

    std::string_view source;
    std::string_view::size_type first;
    std::string_view::size_type last;

    struct end_t {};  // Tag type used to construct the end iterator.
    static constexpr end_t end{};

    constexpr iterator() = default;  // required but not useful

    /// Construct a begin iterator positioned at the first line.
    ///
    constexpr iterator(std::string_view str) noexcept
        : source{str}, first{0}, last(source.find('\n')) {
      if (last == std::string_view::npos) last = source.size();
    }

    /// Construct the end iterator.
    ///
    constexpr iterator(std::string_view str, end_t) noexcept
        : source{str}, first{source.size()}, last{source.size()} {}

    /// Dereference the iterator to return the current line.
    /// The returned view excludes the trailing newline.
    /// If the line ends with '\r', it is trimmed.
    ///
    constexpr auto operator*() const noexcept -> std::string_view {
      // Easy version without taking care of '\r'.
      // return source.substr(first, last - first);

      // Trim a trailing '\r' for CRLF compatibility.
      std::string_view::size_type size = last - first;
      if (size && (source[first + size - 1] == '\r')) --size;
      return source.substr(first, size);
    }

    /// Advance to the next line. The iterator remains
    /// in the end state once past the last line.
    ///
    constexpr auto operator++() -> iterator& {
      if (last == source.size()) {
        first = source.size();
        return *this;
      }
      first = last + 1;
      last = source.find('\n', first);
      if (last == std::string_view::npos) last = source.size();
      return *this;
    }

    // (Required for forward_iterator)
    constexpr auto operator++(int) -> iterator {
      iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    /// Compare iterators by their position.
    ///
    /// It is undefined behavior to compare iterators from different ranges;
    /// we compare only the `first` index to allow for efficient implementation.
    ///
    friend constexpr auto operator==(iterator const& x,
                                     iterator const& y) noexcept {
      return x.first == y.first;
    }
  };

  /// Construct a view over the given text.
  ///
  constexpr string_lines_view(std::string_view s) noexcept : source(s) {}

  /// Return iterator to the first line.
  ///
  constexpr auto begin() const noexcept -> iterator { return iterator{source}; }

  /// Return iterator to one-past-last line.
  ///
  constexpr auto end() const noexcept -> iterator {
    return iterator{source, iterator::end};
  }

  std::string_view source;
};

namespace views {

struct lines_fn {
  static constexpr auto operator()(std::string_view str) noexcept {
    return string_lines_view(str);
  }
  friend constexpr auto operator|(auto&& str, lines_fn) noexcept {
    return string_lines_view(std::forward<decltype(str)>(str));
  }
};

inline constexpr lines_fn lines{};

}  // namespace views

}  // namespace xstd
