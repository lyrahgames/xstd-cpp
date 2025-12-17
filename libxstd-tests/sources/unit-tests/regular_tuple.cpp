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
#include <doctest/doctest.h>
//
#include <xstd/functional/basic_tuple.hpp>
//
#include "log_value.hpp"

using namespace std;
using namespace xstd;

namespace {

struct nothing {};

struct uint8x2 : array<uint8, 2> {};
struct uint8x3 : array<uint8, 3> {};
struct uint8x5 : array<uint8, 5> {};

template <size_t index>
constexpr auto dynamic_offset(auto& x) noexcept {
  return reinterpret_cast<const std::byte*>(std::addressof(value<index>(x))) -
         reinterpret_cast<const std::byte*>(std::addressof(x));
};

}  // namespace

SCENARIO("Size and Alignment of Basic Types") {
  static_assert(sizeof(nothing) == 1);
  static_assert(alignof(nothing) == 1);
  //
  static_assert(sizeof(uint8) == 1);
  static_assert(alignof(uint8) == 1);
  //
  static_assert(sizeof(uint8x2) == 2);
  static_assert(alignof(uint8x2) == 1);
  //
  static_assert(sizeof(uint8x3) == 3);
  static_assert(alignof(uint8x3) == 1);
  //
  static_assert(sizeof(uint8x5) == 5);
  static_assert(alignof(uint8x5) == 1);
}

SCENARIO("Regular Tuple Size, Alignment, and Offset") {
  static_assert(sizeof(regular_tuple<>) == 1);
  static_assert(alignof(regular_tuple<>) == 1);

  static_assert(sizeof(regular_tuple<uint8, uint32>) == 8);
  static_assert(alignof(regular_tuple<uint8, uint32>) == 4);
  //
  static_assert(sizeof(regular_tuple<uint8, uint8, uint32>) == 8);
  static_assert(alignof(regular_tuple<uint8, uint8, uint32>) == 4);

  constexpr auto types = meta::type_list<  //
      nothing,                             //
      uint8, uint8x2, uint8x3, uint8x5,    //
      uint16, uint32, uint64,              //
      float32,                             //
      // float64,
      // string,
      czstring
      // Reference types needs to be removed as `offsetof`
      // is invalid within non-standard-layout type.
      // int&
      >{};

  for_each(types, [&]<typename type> {
    {
      struct test {
        type x;
      };
      using tuple_type = regular_tuple<type>;
      static_assert(sizeof(tuple_type) == sizeof(test));
      static_assert(alignof(tuple_type) == alignof(test));
    }

    for_each(types, [&]<typename type2> {
      struct test {
        type x;
        type2 y;
      };
      using tuple_type = regular_tuple<type, type2>;

      static_assert(sizeof(tuple_type) == sizeof(test));
      static_assert(alignof(tuple_type) == alignof(test));

      static_assert(offsetof(test, x) == tuple_type::template byte_offset<0>());
      static_assert(offsetof(test, y) == tuple_type::template byte_offset<1>());

      constexpr tuple_type x{};
      CHECK((dynamic_offset<0>(x)) == tuple_type::template byte_offset<0>());
      CHECK((dynamic_offset<1>(x)) == tuple_type::template byte_offset<1>());

      for_each(types, [&]<typename type3> {
        struct test {
          type x;
          type2 y;
          type3 z;
        };
        using tuple_type = regular_tuple<type, type2, type3>;

        static_assert(sizeof(tuple_type) == sizeof(test));
        static_assert(alignof(tuple_type) == alignof(test));

        static_assert(offsetof(test, x) ==
                      tuple_type::template byte_offset<0>());
        static_assert(offsetof(test, y) ==
                      tuple_type::template byte_offset<1>());
        static_assert(offsetof(test, z) ==
                      tuple_type::template byte_offset<2>());

        constexpr tuple_type x{};
        CHECK((dynamic_offset<0>(x)) == tuple_type::template byte_offset<0>());
        CHECK((dynamic_offset<1>(x)) == tuple_type::template byte_offset<1>());
        CHECK((dynamic_offset<2>(x)) == tuple_type::template byte_offset<2>());
      });
    });
  });
}

SCENARIO("Regular Tuple Element Access") {
  //   { regular_tuple<> x{}; }

  SUBCASE("Irreducible Types") {
    {
      using tuple_type = std::tuple<int>;
      //
      tuple_type x{1};
      static_assert(std::same_as<int&, decltype(get<0>(x))>);
      static_assert(std::same_as<int&&, decltype(get<0>(std::move(x)))>);
      static_assert(std::same_as<int&&, decltype(get<0>(tuple_type{1}))>);
      CHECK(get<0>(x) == 1);
      ++get<0>(x);
      CHECK(get<0>(x) == 2);
      //
      const tuple_type y{2};
      static_assert(std::same_as<const int&, decltype(get<0>(y))>);
      static_assert(std::same_as<const int&&, decltype(get<0>(std::move(y)))>);
      CHECK(get<0>(y) == 2);
    }
    using tuple_type = regular_tuple<int>;
    //
    tuple_type x{1};
    static_assert(std::same_as<int&, decltype(value<0>(x))>);
    static_assert(std::same_as<int&&, decltype(value<0>(std::move(x)))>);
    static_assert(std::same_as<int&&, decltype(value<0>(tuple_type{1}))>);
    CHECK(value<0>(x) == 1);
    ++value<0>(x);
    CHECK(value<0>(x) == 2);
    //
    const tuple_type y{2};
    static_assert(std::same_as<const int&, decltype(value<0>(y))>);
    static_assert(std::same_as<const int&&, decltype(value<0>(std::move(y)))>);
    CHECK(value<0>(y) == 2);
  }

  SUBCASE("LValue References") {
    {
      using tuple_type = std::tuple<int&>;
      int data = 1;
      tuple_type x{data};
      static_assert(std::same_as<int&, decltype(get<0>(x))>);
      static_assert(std::same_as<int&, decltype(get<0>(std::move(x)))>);
      CHECK(get<0>(x) == 1);
      ++get<0>(x);
      CHECK(get<0>(x) == 2);
      CHECK(data == 2);
      //
      const tuple_type y{data};
      static_assert(std::same_as<int&, decltype(get<0>(y))>);
      static_assert(std::same_as<int&, decltype(get<0>(std::move(y)))>);
      CHECK(get<0>(y) == 2);
      ++get<0>(y);
      CHECK(get<0>(y) == 3);
      CHECK(data == 3);
    }
    using tuple_type = regular_tuple<int&>;
    int data = 1;
    tuple_type x{data};
    static_assert(std::same_as<int&, decltype(value<0>(x))>);
    static_assert(std::same_as<int&, decltype(value<0>(std::move(x)))>);
    CHECK(value<0>(x) == 1);
    ++value<0>(x);
    CHECK(value<0>(x) == 2);
    CHECK(data == 2);
    //
    const tuple_type y{data};
    static_assert(std::same_as<int&, decltype(value<0>(y))>);
    static_assert(std::same_as<int&, decltype(value<0>(std::move(y)))>);
    CHECK(value<0>(y) == 2);
    ++value<0>(y);
    CHECK(value<0>(y) == 3);
    CHECK(data == 3);
  }

  SUBCASE("Const LValue References") {
    {
      using tuple_type = std::tuple<const int&>;
      int data = 1;
      tuple_type x{data};
      static_assert(std::same_as<const int&, decltype(get<0>(x))>);
      static_assert(std::same_as<const int&, decltype(get<0>(std::move(x)))>);
      static_assert(
          std::same_as<const int&, decltype(get<0>(tuple_type{data}))>);
      CHECK(get<0>(x) == 1);
      ++data;
      CHECK(get<0>(x) == 2);
      //
      const tuple_type y{data};
      static_assert(std::same_as<const int&, decltype(get<0>(y))>);
      static_assert(std::same_as<const int&, decltype(get<0>(std::move(y)))>);
      CHECK(get<0>(y) == 2);
      ++data;
      CHECK(get<0>(y) == 3);
    }
    using tuple_type = regular_tuple<const int&>;
    int data = 1;
    tuple_type x{data};
    static_assert(std::same_as<const int&, decltype(value<0>(x))>);
    static_assert(std::same_as<const int&, decltype(value<0>(std::move(x)))>);
    static_assert(
        std::same_as<const int&, decltype(value<0>(tuple_type{data}))>);
    CHECK(value<0>(x) == 1);
    ++data;
    CHECK(value<0>(x) == 2);
    //
    const tuple_type y{data};
    static_assert(std::same_as<const int&, decltype(value<0>(y))>);
    static_assert(std::same_as<const int&, decltype(value<0>(std::move(y)))>);
    CHECK(value<0>(y) == 2);
    ++data;
    CHECK(value<0>(y) == 3);
  }

  SUBCASE("RValue References") {
    {
      using tuple_type = std::tuple<int&&>;
      int data = 1;
      tuple_type x{std::move(data)};
      static_assert(std::same_as<int&, decltype(get<0>(x))>);
      static_assert(std::same_as<int&&, decltype(get<0>(std::move(x)))>);
      //
      const tuple_type y{std::move(data)};
      static_assert(std::same_as<int&, decltype(get<0>(y))>);
      static_assert(std::same_as<int&&, decltype(get<0>(std::move(y)))>);
    }
    using tuple_type = regular_tuple<int&&>;
    int data = 1;
    tuple_type x{std::move(data)};
    static_assert(std::same_as<int&, decltype(value<0>(x))>);
    static_assert(std::same_as<int&&, decltype(value<0>(std::move(x)))>);
    //
    const tuple_type y{std::move(data)};
    static_assert(std::same_as<int&, decltype(value<0>(y))>);
    static_assert(std::same_as<int&&, decltype(value<0>(std::move(y)))>);
  }
}

SCENARIO("Regular Tuple Constructors and Assignments") {
  using tuple_type = regular_tuple<int, string, float>;

  SUBCASE("Default Constructor") {
    tuple_type x{};
    CHECK(value<0>(x) == int{});
    CHECK(value<1>(x) == string{});
    CHECK(value<2>(x) == float{});
  }

  SUBCASE("Forward Constructor") {
    tuple_type x{-1, "Test", 3.14f};
    CHECK(value<0>(x) == -1);
    CHECK(value<1>(x) == "Test");
    CHECK(value<2>(x) == 3.14f);

    SUBCASE("Copy Constructor") {
      tuple_type y = x;
      CHECK(value<0>(y) == -1);
      CHECK(value<1>(y) == "Test");
      CHECK(value<2>(y) == 3.14f);

      SUBCASE("Copy Assignment") {
        value<0>(y) = 2;
        value<1>(y) += " 2";
        value<2>(y) = 1.23f;

        CHECK(value<0>(y) == 2);
        CHECK(value<1>(y) == "Test 2");
        CHECK(value<2>(y) == 1.23f);

        x = y;
        CHECK(value<0>(x) == 2);
        CHECK(value<1>(x) == "Test 2");
        CHECK(value<2>(x) == 1.23f);
      }

      SUBCASE("Move Assignment") {
        value<0>(y) = 2;
        value<1>(y) += " 2";
        value<2>(y) = 1.23f;

        x = move(y);
        CHECK(value<0>(x) == 2);
        CHECK(value<1>(x) == "Test 2");
        CHECK(value<2>(x) == 1.23f);
        //
        // CHECK(value<0>(y) == 2);
        // CHECK(value<1>(y) == "");
        // CHECK(value<2>(y) == 1.23f);
      }
    }

    SUBCASE("Move Constructor") {
      tuple_type y = move(x);
      CHECK(value<0>(y) == -1);
      CHECK(value<1>(y) == "Test");
      CHECK(value<2>(y) == 3.14f);
      //
      // CHECK(value<0>(x) == -1);
      // CHECK(value<1>(x) == "");
      // CHECK(value<2>(x) == 3.14f);
    }
  }

  // SUBCASE("Conversions and Assignments from Convertible Tuples") {
  //   using other_tuple_type = regular_tuple<size_t, czstring, double>;
  //   other_tuple_type x{1, "Hello", 3.14};

  //   // Has to be done explicitly.
  //   tuple_type y{x};
  //   CHECK(value<0>(y) == 1);
  //   CHECK(value<1>(y) == "Hello");
  //   CHECK(value<2>(y) == 3.14f);

  //   value<0>(x) = 2;
  //   value<1>(x) = "Something";
  //   value<2>(x) = 1.23;

  //   y = x;
  //   CHECK(value<0>(y) == 2);
  //   CHECK(value<1>(y) == "Something");
  //   CHECK(value<2>(y) == 1.23f);
  // }
}

// SCENARIO("Regular Tuple Class Template Argument Deduction") {
//   int data = 1;
//   const int cdata = 1;
//   regular_tuple x{data,  'c',    1.23f,     3.14,       "Test",
//                   &data, &cdata, ref(data), cref(data), move(data)};
//   // meta::print_type(x);
//   using tuple_type = decltype(x);
//   static_assert(std::same_as<int, tuple_type::template type<0>>);
//   static_assert(std::same_as<char, tuple_type::template type<1>>);
//   static_assert(std::same_as<float, tuple_type::template type<2>>);
//   static_assert(std::same_as<double, tuple_type::template type<3>>);
//   static_assert(std::same_as<czstring, tuple_type::template type<4>>);
//   static_assert(std::same_as<int*, tuple_type::template type<5>>);
//   static_assert(std::same_as<const int*, tuple_type::template type<6>>);
//   static_assert(std::same_as<int&, tuple_type::template type<7>>);
//   static_assert(std::same_as<const int&, tuple_type::template type<8>>);
//   static_assert(std::same_as<int, tuple_type::template type<9>>);
// }

// SCENARIO("Regular Tuple Structured Bindings") {
//   int data = 1;
//   // int& test = move(data); // This is not allowed.
//   regular_tuple v{data, ref(data), cref(data)};
//   // meta::print_type(v);
//   // Consistency checks, because structured bindings are strange...
//   {
//     auto x = value<0>(v);
//     auto y = value<1>(v);
//     auto z = value<2>(v);
//     static_assert(std::same_as<int, decltype(x)>);
//     static_assert(std::same_as<int, decltype(y)>);
//     static_assert(std::same_as<int, decltype(z)>);
//   }
//   {
//     auto& x = value<0>(v);
//     auto& y = value<1>(v);
//     auto& z = value<2>(v);
//     static_assert(std::same_as<int&, decltype(x)>);
//     static_assert(std::same_as<int&, decltype(y)>);
//     static_assert(std::same_as<const int&, decltype(z)>);
//   }
//   {
//     const auto& x = value<0>(v);
//     const auto& y = value<1>(v);
//     const auto& z = value<2>(v);
//     static_assert(std::same_as<const int&, decltype(x)>);
//     static_assert(std::same_as<const int&, decltype(y)>);
//     static_assert(std::same_as<const int&, decltype(z)>);
//   }
//   //
//   {
//     auto x = value<0>(move(v));
//     auto y = value<1>(move(v));
//     auto z = value<2>(move(v));
//     static_assert(std::same_as<int, decltype(x)>);
//     static_assert(std::same_as<int, decltype(y)>);
//     static_assert(std::same_as<int, decltype(z)>);
//   }
//   {
//     auto&& x = value<0>(move(v));
//     auto&& y = value<1>(move(v));
//     auto&& z = value<2>(move(v));
//     static_assert(std::same_as<int&&, decltype(x)>);
//     static_assert(std::same_as<int&, decltype(y)>);
//     static_assert(std::same_as<const int&, decltype(z)>);
//   }
//   {
//     const auto& x = value<0>(move(v));
//     const auto& y = value<1>(move(v));
//     const auto& z = value<2>(move(v));
//     static_assert(std::same_as<const int&, decltype(x)>);
//     static_assert(std::same_as<const int&, decltype(y)>);
//     static_assert(std::same_as<const int&, decltype(z)>);
//   }

//   // Structured Bindings are really strange...
//   // The STL tuple behaves the same way...
//   //
//   tuple<int, int&, const int&> w{data, data, data};
//   {
//     auto [x, y, z] = w;
//     static_assert(std::same_as<int, decltype(x)>);
//     static_assert(std::same_as<int&, decltype(y)>);
//     static_assert(std::same_as<const int&, decltype(z)>);
//   }
//   {
//     auto [x, y, z] = move(w);
//     static_assert(std::same_as<int, decltype(x)>);
//     static_assert(std::same_as<int&, decltype(y)>);
//     static_assert(std::same_as<const int&, decltype(z)>);
//   }
//   {
//     auto& [x, y, z] = w;
//     static_assert(std::same_as<int, decltype(x)>);
//     static_assert(std::same_as<int&, decltype(y)>);
//     static_assert(std::same_as<const int&, decltype(z)>);
//   }
//   {
//     const auto& [x, y, z] = w;
//     static_assert(std::same_as<const int, decltype(x)>);
//     static_assert(std::same_as<int&, decltype(y)>);
//     static_assert(std::same_as<const int&, decltype(z)>);
//   }
//   // The regular tuple...
//   {
//     auto [x, y, z] = v;
//     static_assert(std::same_as<int, decltype(x)>);
//     static_assert(std::same_as<int&, decltype(y)>);
//     static_assert(std::same_as<const int&, decltype(z)>);
//   }
//   {
//     auto [x, y, z] = move(v);
//     static_assert(std::same_as<int, decltype(x)>);
//     static_assert(std::same_as<int&, decltype(y)>);
//     static_assert(std::same_as<const int&, decltype(z)>);
//   }
//   {
//     auto& [x, y, z] = v;
//     static_assert(std::same_as<int, decltype(x)>);
//     static_assert(std::same_as<int&, decltype(y)>);
//     static_assert(std::same_as<const int&, decltype(z)>);
//   }
//   {
//     const auto& [x, y, z] = v;
//     static_assert(std::same_as<const int, decltype(x)>);
//     static_assert(std::same_as<int&, decltype(y)>);
//     static_assert(std::same_as<const int&, decltype(z)>);
//   }
// }

// SCENARIO("Regular Tuple Swapping") {
//   using tuple_type = regular_tuple<int, char, float>;
//   tuple_type x{-1, 'c', 1.23f};
//   tuple_type y{1, 'x', 3.14f};

//   CHECK(value<0>(x) == -1);
//   CHECK(value<1>(x) == 'c');
//   CHECK(value<2>(x) == 1.23f);
//   CHECK(value<0>(y) == 1);
//   CHECK(value<1>(y) == 'x');
//   CHECK(value<2>(y) == 3.14f);

//   swap(x, y);

//   CHECK(value<0>(x) == 1);
//   CHECK(value<1>(x) == 'x');
//   CHECK(value<2>(x) == 3.14f);
//   CHECK(value<0>(y) == -1);
//   CHECK(value<1>(y) == 'c');
//   CHECK(value<2>(y) == 1.23f);
// }

SCENARIO("Regular Tuple Total Ordering") {
  using tuple_type = regular_tuple<int, char, float>;
  static_assert(tuple_type{-1, 'c', 1.23f} == tuple_type{-1, 'c', 1.23f});
  static_assert(tuple_type{-1, 'c', 1.23f} <= tuple_type{-1, 'c', 1.23f});
  static_assert(tuple_type{-1, 'c', 1.23f} >= tuple_type{-1, 'c', 1.23f});
  static_assert(tuple_type{-1, 'c', 1.23f} != tuple_type{-1, 'c', 3.14f});
  static_assert(tuple_type{-1, 'c', 1.23f} < tuple_type{-1, 'c', 3.14f});
  static_assert(tuple_type{-1, 'c', 1.23f} <= tuple_type{-1, 'c', 3.14f});
  static_assert(tuple_type{-1, 'c', 9.0f} >= tuple_type{-1, 'c', 3.14f});
  static_assert(tuple_type{-1, 'c', 9.0f} > tuple_type{-1, 'c', 3.14f});
}

// // SCENARIO("Regular Tuple Dynamic Memory Layout") {}

// namespace {

// template <typename T>
// struct wrapped {
//   T value;
// };

// template <instance::type_list list, template <typename> typename trait>
// constexpr auto all =
//     list::apply([]<typename... t> { return (trait<t>::value && ...); });

// }  // namespace

SCENARIO("Regular Tuple Preservation of Type Traits") {
  static_assert(is_standard_layout_v<regular_tuple<>>);

  constexpr auto tuples = meta::type_list<regular_tuple<int, string>,       //
                                          regular_tuple<uint8, uint64>,     //
                                          regular_tuple<czstring, string>,  //
                                          regular_tuple<int, float>>{};

  for_each(tuples, []<typename tuple_type> {
    constexpr auto types = tuple_type::types();

    // Default Construction
    static_assert(is_default_constructible_v<tuple_type> ==
                  all_of(types, []<typename type> {
                    return is_default_constructible_v<type>;
                  }));
    static_assert(is_trivially_default_constructible_v<tuple_type> ==
                  all_of(types, []<typename type> {
                    return is_trivially_default_constructible_v<type>;
                  }));
    static_assert(is_nothrow_default_constructible_v<tuple_type> ==
                  all_of(types, []<typename type> {
                    return is_nothrow_default_constructible_v<type>;
                  }));
    // Copy Construction
    static_assert(is_copy_constructible_v<tuple_type> ==
                  all_of(types, []<typename type> {
                    return is_copy_constructible_v<type>;
                  }));
    static_assert(is_trivially_copy_constructible_v<tuple_type> ==
                  all_of(types, []<typename type> {
                    return is_trivially_copy_constructible_v<type>;
                  }));
    static_assert(is_nothrow_copy_constructible_v<tuple_type> ==
                  all_of(types, []<typename type> {
                    return is_nothrow_copy_constructible_v<type>;
                  }));
    // Move Construction
    static_assert(is_move_constructible_v<tuple_type> ==
                  all_of(types, []<typename type> {
                    return is_move_constructible_v<type>;
                  }));
    static_assert(is_trivially_move_constructible_v<tuple_type> ==
                  all_of(types, []<typename type> {
                    return is_trivially_move_constructible_v<type>;
                  }));
    static_assert(is_nothrow_move_constructible_v<tuple_type> ==
                  all_of(types, []<typename type> {
                    return is_nothrow_move_constructible_v<type>;
                  }));
    // Destruction
    static_assert(
        is_destructible_v<tuple_type> ==
        all_of(types, []<typename type> { return is_destructible_v<type>; }));
    static_assert(is_trivially_destructible_v<tuple_type> ==
                  all_of(types, []<typename type> {
                    return is_trivially_destructible_v<type>;
                  }));
    static_assert(is_nothrow_destructible_v<tuple_type> ==
                  all_of(types, []<typename type> {
                    return is_nothrow_destructible_v<type>;
                  }));
    // Copy Assignment
    static_assert(is_copy_assignable_v<tuple_type> ==
                  all_of(types, []<typename type> {
                    return is_copy_assignable_v<type>;
                  }));
    static_assert(is_trivially_copy_assignable_v<tuple_type> ==
                  all_of(types, []<typename type> {
                    return is_trivially_copy_assignable_v<type>;
                  }));
    static_assert(is_nothrow_copy_assignable_v<tuple_type> ==
                  all_of(types, []<typename type> {
                    return is_nothrow_copy_assignable_v<type>;
                  }));
    // Move Assignment
    static_assert(is_move_assignable_v<tuple_type> ==
                  all_of(types, []<typename type> {
                    return is_move_assignable_v<type>;
                  }));
    static_assert(is_trivially_move_assignable_v<tuple_type> ==
                  all_of(types, []<typename type> {
                    return is_trivially_move_assignable_v<type>;
                  }));
    static_assert(is_nothrow_move_assignable_v<tuple_type> ==
                  all_of(types, []<typename type> {
                    return is_nothrow_move_assignable_v<type>;
                  }));
    // Swap
    static_assert(
        is_swappable_v<tuple_type> ==
        all_of(types, []<typename type> { return is_swappable_v<type>; }));
    static_assert(is_nothrow_swappable_v<tuple_type> ==
                  all_of(types, []<typename type> {
                    return is_nothrow_swappable_v<type>;
                  }));
    //
    // GCC fulfills assertions for triviality. Clang throws an error.
    // static_assert(is_trivial_v<tuple_type> == all_of(types, is_trivial));
    // static_assert(is_trivial_v<tuple_type> ==
    //               is_trivial_v<typename tuple_type::tuple_type>);
    // static_assert(is_standard_layout_v<tuple_type> ==
    //               all_of(types, is_standard_layout));
    // static_assert(is_trivially_copyable_v<tuple_type> ==
    //               all_of(types, is_trivially_copyable));
  });
}

// // SCENARIO("Constexpr Regular Tuple") {}

// SCENARIO("Regular Tuple Type Logs") {
//   struct log::state log_int_state {};
//   using log_int = basic_log_value<int, unique_log>;

//   struct log::state log_string_state {};
//   using log_string = basic_log_value<string, unique_log>;

//   {
//     reset(log_int::log);
//     reset(log_string::log);

//     {
//       regular_tuple<log_int, log_int, log_string> x{1, 2, "Test"};

//       CHECK(value<0>(x).value == 1);
//       CHECK(value<1>(x).value == 2);
//       CHECK(value<2>(x).value == string("Test"));

//       // Check that only constructors have been called.
//       log_int_state.counters[log_int_state.construct_calls] = 2;
//       CHECK(log_int::log == log_int_state);
//       log_string_state.counters[log_string_state.construct_calls] = 1;
//       CHECK(log_string::log == log_string_state);

//       regular_tuple<log_int&, const log_int&, log_string&> y{x};

//       CHECK(log_int::log == log_int_state);
//       CHECK(log_string::log == log_string_state);

//       CHECK(value<0>(x).value == 1);
//       CHECK(value<1>(x).value == 2);
//       CHECK(value<2>(x).value == string("Test"));

//       CHECK(value<0>(y).value == 1);
//       CHECK(value<1>(y).value == 2);
//       CHECK(value<2>(y).value == string("Test"));

//       ++value<0>(y).value;

//       CHECK(value<0>(x).value == 2);
//       CHECK(value<0>(y).value == 2);

//       --value<1>(x).value;

//       CHECK(value<1>(x).value == 1);
//       CHECK(value<1>(y).value == 1);

//       value<2>(x).value += " more";
//       value<2>(y).value += " and even more";

//       CHECK(value<2>(x).value == "Test more and even more");
//       CHECK(value<2>(y).value == "Test more and even more");

//       // regular_tuple<log_int, log_int, log_string> z{
//       //     move(value<0>(y)), move(value<1>(y)), move(value<2>(y))};
//       regular_tuple<log_int, log_int, log_string> z{std::move(y)};
//       // regular_tuple<log_int, log_int, log_string> z{};
//       // z = std::move(y);

//       log_int_state.counters[log_int_state.copy_construct_calls] += 2;
//       CHECK(log_int::log == log_int_state);

//       log_string_state.counters[log_string_state.copy_construct_calls] += 1;
//       CHECK(log_string::log == log_string_state);

//       CHECK(value<0>(x).value == 2);
//       CHECK(value<0>(y).value == 2);
//       CHECK(value<0>(z).value == 2);

//       CHECK(value<1>(x).value == 1);
//       CHECK(value<1>(y).value == 1);
//       CHECK(value<1>(z).value == 1);

//       CHECK(value<2>(x).value == "Test more and even more");
//       CHECK(value<2>(y).value == "Test more and even more");
//       CHECK(value<2>(z).value == "Test more and even more");

//       ++value<0>(y).value;
//       --value<1>(x).value;

//       CHECK(value<0>(x).value == 3);
//       CHECK(value<0>(y).value == 3);
//       CHECK(value<0>(z).value == 2);

//       CHECK(value<1>(x).value == 0);
//       CHECK(value<1>(y).value == 0);
//       CHECK(value<1>(z).value == 1);

//       CHECK(value<2>(x).value == "Test more and even more");
//       CHECK(value<2>(y).value == "Test more and even more");
//       CHECK(value<2>(z).value == "Test more and even more");

//       //       CHECK(value<0>(z).value == 2);
//       //       CHECK(value<1>(z).value == 1);
//       //       CHECK(value<2>(z).value == string("Test more and even more"));

//       {
//         const auto& [a, b, c] = z;

//         CHECK(log_int::log == log_int_state);
//         CHECK(log_string::log == log_string_state);

//         CHECK(a.value == 2);
//         CHECK(b.value == 1);
//         CHECK(c.value == string("Test more and even more"));
//       }

//       regular_tuple<log_int, log_int, log_string> v{10, 20, "test"};

//       log_int_state.counters[log_int_state.construct_calls] += 2;
//       CHECK(log_int::log == log_int_state);
//       log_string_state.counters[log_string_state.construct_calls] += 1;
//       CHECK(log_string::log == log_string_state);

//       {
//         const auto& [a, b, c] = v;
//         CHECK(a.value == 10);
//         CHECK(b.value == 20);
//         CHECK(c.value == string("test"));
//       }

//       // Cannot rassign y due to references.
//       // y = v;
//       regular_tuple<log_int&, const log_int&, log_string&> w{v};

//       CHECK(log_int::log == log_int_state);
//       CHECK(log_string::log == log_string_state);

//       {
//         const auto& [a, b, c] = w;
//         CHECK(a.value == 10);
//         CHECK(b.value == 20);
//         CHECK(c.value == string("test"));
//       }

//       z = w;

//       log_int_state.counters[log_int_state.copy_assign_calls] += 2;
//       CHECK(log_int::log == log_int_state);
//       log_string_state.counters[log_string_state.copy_assign_calls] += 1;
//       CHECK(log_string::log == log_string_state);
//     }
//   }
// }

// static_assert(!generic::tuple<int>);
// static_assert(!generic::tuple<type_list<char, float>>);
// static_assert(!generic::tuple<value_list<1, 'c'>>);
// static_assert(!generic::tuple<std::vector<int>>);
// static_assert(!generic::tuple<std::string>);
// //
// static_assert(generic::tuple<std::array<int, 10>>);
// static_assert(generic::tuple<std::pair<int, int>>);
// static_assert(generic::tuple<std::tuple<>>);
// static_assert(generic::tuple<xstd::regular_tuple<char, float>>);
