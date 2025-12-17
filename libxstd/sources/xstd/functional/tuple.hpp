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
#include <xstd/functional/defaults.hpp>
//
#include <xstd/meta/index_list.hpp>
#include <xstd/meta/type_list.hpp>

namespace xstd {

/// Check whether there exists a specialization of `std::tuple_size`
/// for `type` that returns a value representing a size.
///
template <typename type>
concept has_std_tuple_size = requires {
  { std::tuple_size<type>::value } -> std::convertible_to<size_t>;
};

/// Check whether there exists a specialization of `std::tuple_element`
/// for `type` and `index` that returns a type.
///
template <typename type, size_t index>
concept has_std_tuple_element_for =
    requires { typename std::tuple_element<index, type>::type; };

/// Check whether there exist specializations of `std::tuple_element`
/// for `type` and all indices in the range [0,`std::tuple_size<type>::value`).
///
template <typename type>
concept std_tuple_element_accessible =
    has_std_tuple_size<type> &&
    all_of(meta::indices_less_than<std::tuple_size<type>::value>(),
           []<size_t index> { return has_std_tuple_element_for<type, index>; });

/// Check whether there is an overload of `get` that can be found by ADL and
/// invoked with `index` as template argument and `type` as function argument.
/// `get<index>(type)` must return a type that is convertible
/// to the standard tuple element of `type` at `index`.
///
template <typename type, size_t index>
concept std_tuple_gettable_for =
    has_std_tuple_element_for<std::remove_cvref_t<type>, index> &&  //
    requires(type value) {
      {
        get<index>(value)
      } -> std::convertible_to<  //
          typename std::tuple_element<index, std::remove_cvref_t<type>>::type>;
    };

/// Check whether an overload of `get` can be found by ADL and invoked
/// with `type` as function argument and for all indices in the range
/// [0,`std::tuple_size<type>::value`) as template argument.
/// The return types of `get` must be convertible
/// to the standard tuple element types of `type`.
///
template <typename type>
concept std_tuple_gettable =
    std_tuple_element_accessible<std::remove_cvref_t<type>> &&
    all_of(meta::indices_less_than<
               std::tuple_size<std::remove_cvref_t<type>>::value>(),
           []<size_t index> { return std_tuple_gettable_for<type, index>; });

/// Check whether a given type fulfills the criteria of generic tuple or tuple-like.
/// In that case, the type can be used with tuple-like structured bindings.
///
template <typename type>
concept generic_tuple = std_tuple_gettable<type>;
template <typename type>
concept tuple_like = std_tuple_gettable<type>;

/// Convenience wrapper function to access
/// `std::tuple_size` for generic tuples.
///
template <tuple_like tuple>
consteval auto tuple_size() noexcept {
  return std::tuple_size<std::remove_cvref_t<tuple>>::value;
}

/// Convenience wrapper function to access
/// `std::tuple_element` for generic tuples.
///
template <size_t index, tuple_like tuple>
consteval auto tuple_element() noexcept {
  return meta::as_value<
      typename std::tuple_element<index, std::remove_cvref_t<tuple>>::type>;
}

/// Get access to a generic tuple's element at index `index`.
/// This function is a convenient wrapper for `std::get`.
///
template <size_t index>
constexpr auto&& at(tuple_like auto&& t) noexcept(
    noexcept(get<index>(std::forward<decltype(t)>(t)))) {
  return get<index>(std::forward<decltype(t)>(t));
}

/// Return all indices of the tuple-like type
/// `tuple` as a `meta::index_list` object.
///
template <tuple_like tuple>
consteval auto indices() noexcept {
  return meta::indices_less_than<tuple_size<tuple>()>();
}

/// Return all types of the tuple-like type
/// `tuple` as a `meta::type_list` object.
///
template <tuple_like tuple>
consteval auto types() noexcept {
  return fold(indices<tuple>(), []<size_t... indices> {
    return meta::type_list<meta::as_type<tuple_element<indices, tuple>()>...>{};
  });
}

/// Check whether the type `functor` is a callable that can be
/// invoked with all elements of the tuple-like type `tuple`.
///
template <typename functor, typename tuple>
concept tuple_functor_for =
    tuple_like<tuple> && all_of(indices<tuple>(), []<size_t index> {
      return std::invocable<functor,
                            decltype(at<index>(std::declval<tuple>()))>;
    });

/// Check whether the type `functor` is a callable that can be invoked
/// with all elements of the tuple-like type `tuple` as function arguments
/// and their respective indices as template argument.
///
template <typename functor, typename tuple, size_t index>
concept tuple_index_functor_for_index = requires(functor f, tuple t) {
  f.template operator()<index>(at<index>(t));
};
//
template <typename functor, typename tuple>
concept tuple_index_functor_for =
    tuple_like<tuple> && all_of(indices<tuple>(), []<size_t index> {
      return tuple_index_functor_for_index<functor, tuple, index>;
    });

/// Invoke the given functor for each element of the tuple.
/// The functor is simply a unary callable that can be
/// invoked with all element types of the given tuple.
///
/// Example Lambda Expression:
/// for_each(tuple, [](auto value){ ... });
///
template <tuple_like tuple, tuple_functor_for<tuple> functor>
constexpr void for_each(tuple&& t, functor&& f) {
  for_each(indices<tuple>(),
           [&]<size_t index> { f(at<index>(std::forward<tuple>(t))); });
}

/// Invoke the given functor for each element of the tuple.
/// The invocation will instantiate the `operator()`
/// with the element's index as template argument.
///
/// Example Lambda Expression:
/// for_each(tuple, []<size_t index>(auto value){ ... });
///
template <tuple_like tuple, tuple_index_functor_for<tuple> functor>
constexpr void for_each(tuple&& t, functor&& f) {
  for_each(indices<tuple>(), [&]<size_t index> {
    f.template operator()<index>(at<index>(std::forward<tuple>(t)));
  });
}

/// Invoke the callable object `f` with the elements of `t` as arguments.
/// Similar to `std::apply` but for all tuple-like types.
///
template <typename functor, tuple_like tuple>
constexpr decltype(auto) tuple_invoke(functor&& f, tuple&& t) {
  return fold(indices<tuple>(), [&]<size_t... is> {
    return std::invoke(std::forward<functor>(f),
                       at<is>(std::forward<tuple>(t))...);
  });
}

namespace detail {

template <typename tuple_type, size_t index>
concept tuple_get_access = requires(tuple_type value) {
  {
    get<index>(value)
  }
  -> std::convertible_to<typename std::tuple_element<index, tuple_type>::type>;
};

template <typename tuple_type, size_t index>
concept xstd_tuple_value_access = requires(tuple_type tuple) {
  {
    value<index>(tuple)
  } -> std::convertible_to<typename tuple_type::template type<index>>;
  // Only ask for specific type.
  // For an array, you would not want to provide a type list.
};

}  // namespace detail

/// Checks whether a given type fulfills the requirements of a generic tuple.
///
// template <typename tuple_type>
// concept generic_tuple = all_of(
//     meta::index_list_from_iota<std::tuple_size<tuple_type>::value>(),
//     []<size_t index> { return detail::tuple_get_access<tuple_type, index>; });

///
///
template <typename T>
concept generic_reducible_tuple = generic_tuple<std::remove_cvref_t<T>>;

///
///
template <typename tuple_type>
concept generic_xstd_tuple =
    all_of(meta::index_list_from_iota<tuple_type::size()>(),  //
           []<size_t index> {
             return detail::xstd_tuple_value_access<tuple_type, index>;
           });

///
///
template <typename tuple_type>
concept generic_reducible_xstd_tuple =
    generic_xstd_tuple<std::decay_t<tuple_type>>;

///
///
// A tuple type is not a tag type and as such should be given as template argument.
template <generic_tuple tuple_type, size_t... indices>
consteval auto type_list_from(meta::index_list<indices...>) {
  return meta::type_list<
      typename std::tuple_element<indices, tuple_type>::type...>{};
}
//
template <generic_tuple tuple_type>
consteval auto type_list_from() {
  return type_list_from<tuple_type>(
      meta::index_list_from_iota<std::tuple_size<tuple_type>::value>());
}

///
///
// template <size_t... indices>
// constexpr auto for_each(generic_reducible_tuple auto&& tuple,
//                         auto&& f,
//                         meta::index_list<indices...>) {
//   (f(get<indices>(std::forward<decltype(tuple)>(tuple))), ...);
// }
// //
// constexpr auto for_each(generic_reducible_tuple auto&& tuple, auto&& f) {
//   using tuple_type = std::decay_t<decltype(tuple)>;
//   for_each(std::forward<decltype(tuple)>(tuple), std::forward<decltype(f)>(f),
//            meta::index_list_from_iota<std::tuple_size<tuple_type>::value>());
// }

///
///
template <typename type>
concept tuple_representable =
    tuple_like<typename type::tuple_representation> &&
    std::derived_from<type, typename type::tuple_representation>;

template <typename type>
concept tuple_castable = requires(type value) {
  { value.as_tuple() } -> tuple_like;
};
template <tuple_castable type>
using tuple_cast_type =
    std::remove_cvref_t<decltype(std::declval<type>().as_tuple())>;
///
template <tuple_like tuple>
struct tuple_likeness : tuple {
  using tuple_representation = tuple;
  using tuple::tuple;
};

template <size_t index>
constexpr decltype(auto) get(tuple_castable auto&& t) noexcept(
    noexcept(get<index>(std::forward<decltype(t)>(t).as_tuple()))) {
  return get<index>(std::forward<decltype(t)>(t).as_tuple());
}

}  // namespace xstd

namespace std {

template <xstd::tuple_castable wrapper>
struct tuple_size<wrapper> {
  static constexpr size_t value =
      std::tuple_size<xstd::tuple_cast_type<wrapper>>::value;
};
template <size_t index, xstd::tuple_castable wrapper>
struct tuple_element<index, wrapper> {
  using type =
      typename std::tuple_element<index, xstd::tuple_cast_type<wrapper>>::type;
};

template <xstd::tuple_representable wrapper>
struct tuple_size<wrapper> {
  static constexpr size_t value =
      std::tuple_size<typename wrapper::tuple_representation>::value;
};
template <size_t index, xstd::tuple_representable wrapper>
struct tuple_element<index, wrapper> {
  using type =
      typename std::tuple_element<index,
                                  typename wrapper::tuple_representation>::type;
};

// template <xstd::meta::name_list_instance names,
//           xstd::generic_tuple tuple_type>
// struct tuple_size<xstd::named_tuple<names, tuple_type>> {
//   static constexpr size_t value = std::tuple_size<tuple_type>::value;
// };

// template <size_t index,
//           xstd::meta::name_list_instance names,
//           xstd::generic_tuple tuple_type>
// struct tuple_element<index, xstd::named_tuple<names, tuple_type>> {
//   using type = typename std::tuple_element<index, tuple_type>::type;
// };

}  // namespace std
