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
#include <xstd/functional/tuple.hpp>

using namespace xstd;

SCENARIO("") {
  static_assert(tuple_like<std::tuple<>>);
  static_assert(tuple_like<std::tuple<int>>);
  static_assert(tuple_like<std::tuple<int, float>>);
  //
  static_assert(tuple_like<std::tuple<int>&>);
  static_assert(tuple_like<std::tuple<int, float> const&>);
  static_assert(tuple_like<std::tuple<int, float>&&>);
  static_assert(tuple_like<std::tuple<int, float> const&&>);
  //
  static_assert(tuple_like<std::array<int, 0>>);
  static_assert(tuple_like<std::array<int, 5>>);
  static_assert(tuple_like<std::array<float, 0>>);
  static_assert(tuple_like<std::array<float, 5>>);

  static_assert(types<std::tuple<>>() == meta::type_list<>{});
  static_assert(types<std::tuple<int>>() == meta::type_list<int>{});
  static_assert(types<std::tuple<int&>>() == meta::type_list<int&>{});
  static_assert(types<std::tuple<int&, float>>() ==
                meta::type_list<int&, float>{});
  //
  static_assert(types<std::array<int, 0>>() == meta::type_list<>{});
  static_assert(types<std::array<int, 5>>() ==
                meta::type_list<int, int, int, int, int>{});

  static_assert(type_list_from<std::tuple<>>() == meta::type_list<>{});
  static_assert(type_list_from<std::tuple<int>>() == meta::type_list<int>{});
  static_assert(type_list_from<std::tuple<int&>>() == meta::type_list<int&>{});
  static_assert(type_list_from<std::tuple<int&, float>>() ==
                meta::type_list<int&, float>{});
  //
  static_assert(type_list_from<std::array<int, 0>>() == meta::type_list<>{});
  static_assert(type_list_from<std::array<int, 5>>() ==
                meta::type_list<int, int, int, int, int>{});
}

SCENARIO("") {
  using tuple_type = std::tuple<int, float>;
  {
    auto [a, b] = tuple_type{-1, 1.23f};
    CHECK(a == -1);
    CHECK(b == 1.23f);
  }

  struct test_type : tuple_likeness<tuple_type> {
    using base = tuple_likeness<tuple_type>;
    using base::base;
    // using tuple_type::tuple_type;
    // using tuple_representation = tuple_type;
  };

  static_assert(tuple_like<test_type>);

  struct test_type2 : test_type, std::tuple<char, int> {
    using test_type::test_type;
    using tuple_representation = void;
    auto& as_tuple() const { return static_cast<const test_type&>(*this); }
  };

  static_assert(tuple_like<test_type2>);

  {
    CHECK(get<0>(test_type{-1, 1.23f}) == -1);
    const auto& [a, b] = test_type2{-1, 1.23f};
    CHECK(a == -1);
    CHECK(b == 1.23f);
  }
}

SCENARIO("Tuples: Value Access by `at`") {
  {
    std::tuple<int> t{1};
    CHECK(at<0>(t) == 1);
  }
  {
    std::tuple<int, float> t{-1, 1.23f};
    CHECK(at<0>(t) == -1);
    CHECK(at<1>(t) == 1.23f);
  }
}

SCENARIO("`for_each` Algorithm for Generic Tuples") {
  std::tuple t{-1, 'c', 1.23f};

  for_each(t, [](auto& x) { x += 1; });

  CHECK(at<0>(t) == 0);
  CHECK(at<1>(t) == 'd');
  CHECK(at<2>(t) == 2.23f);

  for_each(t, []<size_t index>(auto& x) { x -= index; });

  CHECK(at<0>(t) == 0);
  CHECK(at<1>(t) == 'c');
  // CHECK(at<2>(t) == 0.23f);

  auto y = tuple_invoke([](auto... x) { return (x + ...); },
                        std::tuple{-1, 'c', 1.23f});
  CHECK(y == 99.23f);
}
