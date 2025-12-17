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
#include <xstd/functional/match.hpp>
#include <xstd/functional/tuple.hpp>
#include <xstd/memory/defaults.hpp>
#include <xstd/meta/permutation.hpp>

namespace xstd {

template <typename... ts>
struct basic_tuple_traits {
  static consteval auto types() noexcept { return meta::type_list<ts...>{}; }

  template <size_t index>
  struct wrapper {
    using type = meta::as_type<element<index>(types())>;

    // constexpr auto&& value(this auto&& self) noexcept {
    //   return std::forward_like<decltype(self)>(self.data);
    // }

    constexpr decltype(auto) value() & noexcept {
      return static_cast<type&>(data);
    }
    constexpr decltype(auto) value() const& noexcept {
      return static_cast<const type&>(data);
    }
    constexpr decltype(auto) value() && noexcept {
      return static_cast<type&&>(data);
    }
    constexpr decltype(auto) value() const&& noexcept {
      return static_cast<const type&&>(data);
    }

    friend constexpr auto operator<=>(const wrapper&,
                                      const wrapper&) noexcept = default;

    type data;
  };
};

template <meta::type_list_instance types,
          meta::permutation_for<size(types{})> =
              decltype(meta::permutation_identity<size(types{})>())>
struct basic_tuple;

template <typename... ts, size_t... indices>
struct basic_tuple<meta::type_list<ts...>, meta::permutation<indices...>>
    : basic_tuple_traits<ts...>::template wrapper<indices>... {
  using traits_type = basic_tuple_traits<ts...>;

  static consteval auto types() noexcept { return meta::type_list<ts...>{}; }

  friend constexpr auto operator<=>(const basic_tuple&,
                                    const basic_tuple&) noexcept = default;

  static consteval auto permutation() noexcept {
    return meta::permutation<indices...>{};
  }

  template <typename base>
  constexpr auto forward_as() & noexcept -> base& {
    return *this;
  }
  template <typename base>
  constexpr auto forward_as() const& noexcept -> const base& {
    return *this;
  }
  template <typename base>
  constexpr auto forward_as() && noexcept -> base&& {
    return std::move(*this);
  }
  template <typename base>
  constexpr auto forward_as() const&& noexcept -> const base&& {
    return std::move(*this);
  }

  template <size_t index>
  constexpr auto&& value(this auto&& self) noexcept {
    return std::forward<decltype(self)>(self)
        .template forward_as<typename traits_type::template wrapper<index>>()
        .value();
  }

  /// Returns the byte offset of an element given by index
  /// inside the tuple structure.
  ///
  template <size_t index>
  static consteval auto byte_offset() noexcept -> size_t {
    if constexpr (index == 0)
      return 0;
    else
      return aligned_offset(
          byte_offset<index - 1>() +
              sizeof(typename traits_type::template wrapper<index - 1>),
          alignof(typename traits_type::template wrapper<index>));
  }
};

template <typename... types>
basic_tuple(types&&...) -> basic_tuple<std::unwrap_ref_decay_t<types>...>;

template <typename... types>
using regular_tuple = basic_tuple<meta::type_list<types...>>;

// template <typename... ts>
// struct regular_tuple : basic_tuple_traits<ts...>::base {
//   static consteval auto types() noexcept { return meta::type_list<ts...>{}; }
// };

template <typename type>
concept regular_tuple_instance =
    matches<type, []<typename... types>(const regular_tuple<types...>&) {
      return meta::as_signature<true>;
    }>;

template <size_t index>
constexpr auto&& value(regular_tuple_instance auto&& t) noexcept {
  return std::forward<decltype(t)>(t).template value<index>();
}

}  // namespace xstd
