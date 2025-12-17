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

namespace xstd::meta {

/// Check whether a given type is a tag type,
/// i.e. semi-regular and empty.
///
template <typename type>
concept generic_tag = std::semiregular<type> && std::is_empty_v<type>;

/// Check whether a given type is a tag type that wraps another type.
/// The wrapped type must be accessible by the member type `type`.
///
template <typename type>
concept generic_type_tag =
    generic_tag<type> && requires { typename type::type; };

/// Check whether a given type is a tag type that wraps a constexpr value.
/// The wrapped value must be accessible by the static member value `value`.
///
template <typename type>
concept generic_value_tag = generic_tag<type> && requires { type::value; };

/// Instances of `type_tag` are tag types that wrap
/// the type that was given as template argument.
/// Types cannot be returned by standard functions directly.
/// Instead, `type_tag` can be used to wrap a type
/// and generate a value by default construction.
/// Using this approach, allows to use types as values.
/// Especially, for `value_list` instances and comparison.
/// The wrapped type can be accessed by the member type `type`.
///
template <typename x>
struct type_tag {
  using type = x;
};

namespace detail {

template <typename type>
struct is_type_tag : std::false_type {};

template <typename type>
struct is_type_tag<type_tag<type>> : std::true_type {};

}  // namespace detail

/// Check whether a given type is an instance of `type_tag`.
///
template <typename type>
concept type_tag_instance = detail::is_type_tag<type>::value;

/// Instances of `value_tag` are tag types that wrap
/// the value that was given as template argument.
/// The `value_tag` is similar to `std::integral_constant`
/// and mainly used to transform values into types.
/// This allows to include values in function signatures.
/// The wrapped value can be accessed by the static member `value`.
///
template <auto x>
struct value_tag {
  static constexpr auto value = x;
};

namespace detail {

template <typename type>
struct is_value_tag : std::false_type {};

template <auto value>
struct is_value_tag<value_tag<value>> : std::true_type {};

}  // namespace detail

/// Check whether the given type is an instance of `value_tag`.
///
template <typename type>
concept value_tag_instance = detail::is_value_tag<type>::value;

/// Generic Equality for Tag Types
///
/// Check whether two given tag types are the same.
/// As a tag type only consists of a single valid state,
/// the comparison reduces to checking type equality.
/// However, integral constants are automatically checked
/// for strict equality, i.e., equality of type and value.
/// Furthermore, the operator== is more intuitive for equality checks.
/// Note, this operator overload will has to be made
/// available by ADL or namespace using directive.
///
consteval bool operator==(generic_tag auto x, generic_tag auto y) noexcept {
  return false;
}
//
template <generic_tag tag>
consteval bool operator==(tag x, tag y) noexcept {
  return true;
}

/// Generic Inequality for Tag Types
///
/// Check whether two given tag types are not equal.
/// Note, this operator overload will has to be made
/// available by ADL or namespace using directive.
///
consteval bool operator!=(generic_tag auto x, generic_tag auto y) noexcept {
  return !(x == y);
}

// Implementation Note:
//
// The following functions should be implemented as meta functions
// because function templates would require additional trailing parentheses.
// Instances of `type_tag` and `value_tag` are the only types for which
// elements can be emplaced and unwrapped by `as_value` and `as_type`.
// Using this for other tag types that might wrap types would get too complicated.
// Also, a tag type might provide access to `type` and `value` at the same time.

// Implementation Note:
//
// We could use `value_from`, `type_from`, and `signature_from`
// instead of the names `as_value`, `as_type`, and `as_signature`.
// However, the `_from` suffix is used for constructor extensions
// and sounds like an involved transformation.
// On the other hand, the `as_` prefix actually sounds like a
// re-interpretation of the given argument and expresses more clearly
// the actual intent of the following meta functions.

namespace detail {

template <typename type>
struct as_value {
  static constexpr auto value = type_tag<type>{};
};
//
template <generic_tag type>
struct as_value<type> {
  static constexpr auto value = type{};
};
//
template <auto x>
struct as_value<value_tag<x>> {
  static constexpr auto value = x;
};

// template <auto value>
// struct as_type {
//   using type = value_tag<value>;
// };
//
// template <generic_tag auto value>
// struct as_type<value> {
//   using type = decltype(value);
// };
//
// The compiler will complain about ambiguity of specializations
// if `type_tag_instance` is used directly as constraining concept.
// Providing the same template signature (template parameter list)
// with an additional `requires` clause correctly marks this as
// a further template specialization for the compiler.
// Also, matching of values with default constructors `type_tag<x>{}`
// is not able to correctly deduce template parameters.
// template <generic_tag auto value>
//   requires type_tag_instance<decltype(value)>
// struct as_type<value> {
//   using type = typename decltype(value)::type;
// };

// Implementation Note:
//
// Using partial template specializations for `constexpr` values
// does not seem to be robust and leads to compile errors for Clang.
// Instead, use functions in an unevaluated context.

template <auto value>
auto as_type() -> typename decltype(value)::type
  requires type_tag_instance<decltype(value)>;

template <auto value>
auto as_type() -> decltype(value)
  requires generic_tag<decltype(value)> &&  //
           (!type_tag_instance<decltype(value)>);

template <auto value>
auto as_type() -> value_tag<value>;

// template <auto x>
// struct as_signature {
//   static constexpr auto value = value_tag<x>{};
// };
//
// template <generic_tag auto x>
// struct as_signature<x> {
//   static constexpr auto value = x;
// };

template <auto x>
consteval auto as_signature() {
  if constexpr (generic_tag<decltype(x)>)
    return x;
  else
    return value_tag<x>{};
}

}  // namespace detail

/// Interpret a given type as value that
/// can be returned in a `consteval` context.
/// If the given type is an instance of `value_tag`,
/// return the wrapped value directly.
/// If the given type is tag type, return a
/// value of this type by default construction.
/// In all other cases, wrap the type by `type_tag`
/// and return a value by default construction.
///
template <typename type>
inline constexpr auto as_value = detail::as_value<type>::value;

/// Interpret a given consteval value as type.
/// If the type of the given value is an instance
/// of `type_tag` return the wrapped type directly.
/// If the type of the given value is a tag type,
/// return the tag type itself, i.e., `decltype(value)`.
/// In all other cases, return wrap the value by
/// `value_tag` and return the respective type.
///
template <auto value>
using as_type = decltype(detail::as_type<value>());
// typename detail::as_type<value>::type;

/// Interpret a given `consteval` value, such that will be
/// part of a function's signature when used as return value.
/// If the type of the given value is already a tag type,
/// return the value itself without modification.
/// In all other cases, wrap the value by `value_tag` and
/// return a new value by using default construction.
///
template <auto value>
inline constexpr auto as_signature = detail::as_signature<value>();
// detail::as_signature<value>::value;

}  // namespace xstd::meta
