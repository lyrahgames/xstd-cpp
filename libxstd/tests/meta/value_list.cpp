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
#include <print>
#include <thread>
//
#include <xstd/functional/match.hpp>
#include <xstd/meta/type_list.hpp>
#include <xstd/meta/value_list.hpp>

// Only the template itself needs to be available
// to access all its functions by making use of ADL.
//
using xstd::meta::value_list;
using xstd::meta::value_list_instance;

// Additional utilities for testing.
//
using xstd::match;
using xstd::meta::as_signature;
using xstd::meta::as_type;
using xstd::meta::as_value;
using xstd::meta::type_list;
using xstd::meta::type_tag;
using xstd::meta::value_tag;

// Check if a given type is an instance of the 'value_list' template.
//
static_assert(!value_list_instance<int>);
static_assert(!value_list_instance<float>);
static_assert(!value_list_instance<char>);
//
static_assert(!value_list_instance<std::string>);
static_assert(!value_list_instance<std::thread>);
//
static_assert(value_list_instance<value_list<>>);
static_assert(value_list_instance<value_list<-1>>);
static_assert(value_list_instance<value_list<'c', -1>>);
static_assert(value_list_instance<value_list<-1, 1u, 'c'>>);
static_assert(
    value_list_instance<value_list<-1, 1u, 'c', value_list<-1, 'c'>{}>>);

// Get the number of types inside a type list.
//
static_assert(size(value_list<>{}) == 0);
static_assert(size(value_list<-1>{}) == 1);
static_assert(size(value_list<'c'>{}) == 1);
static_assert(size(value_list<-1, 'c'>{}) == 2);
static_assert(size(value_list<-1, 'c', 1u>{}) == 3);

// Check whether a 'value_list' instance contains no values.
//
static_assert(empty(value_list<>{}));
static_assert(!empty(value_list<-1>{}));
static_assert(!empty(value_list<-1, 'c'>{}));
static_assert(!empty(value_list<-1, 'c', 1u>{}));

// Access values of a 'value_list' instance by their index.
//
static_assert(element<0>(value_list<-1>{}) == -1);
static_assert(element<0>(value_list<-1, 'c'>{}) == -1);
static_assert(element<1>(value_list<-1, 'c'>{}) == 'c');
static_assert(element<0>(value_list<2u, -1, 'c'>{}) == 2u);
static_assert(element<1>(value_list<2u, -1, 'c'>{}) == -1);
static_assert(element<2>(value_list<2u, -1, 'c'>{}) == 'c');

// Access the first value of a `value_list` instance by using `front`.
//
static_assert(front(value_list<-1>{}) == -1);
static_assert(front(value_list<-1, 'c'>{}) == -1);
static_assert(front(value_list<-1, 'c'>{}) == -1);
static_assert(front(value_list<2u, -1, 'c'>{}) == 2u);
static_assert(front(value_list<2u, -1, 'c'>{}) == 2u);
static_assert(front(value_list<2u, -1, 'c'>{}) == 2u);

// Access the first slice of a `value_list` instance by using `operator*`.
//
static_assert(*value_list<-1>{} == value_list<-1>{});
static_assert(*value_list<-1, 'c'>{} == value_list<-1>{});
static_assert(*value_list<-1, 'c'>{} == value_list<-1>{});
static_assert(*value_list<2u, -1, 'c'>{} == value_list<2u>{});
static_assert(*value_list<2u, -1, 'c'>{} == value_list<2u>{});
static_assert(*value_list<2u, -1, 'c'>{} == value_list<2u>{});

// Access the last value of a `value_list` instance by using `back`.
//
static_assert(back(value_list<-1>{}) == -1);
static_assert(back(value_list<-1, 'c'>{}) == 'c');
static_assert(back(value_list<-1, 'c'>{}) == 'c');
static_assert(back(value_list<2u, -1, 'c'>{}) == 'c');
static_assert(back(value_list<2u, -1, 'c'>{}) == 'c');
static_assert(back(value_list<2u, -1, 'c'>{}) == 'c');

// Create simple value predicate that checks whether
// a compile-time constant is greater than one.
//
constexpr auto greater_one = []<auto x> { return x > 1; };

// Check if a condition holds for all types.
//
static_assert(all_of(value_list<>{}, greater_one));
static_assert(!all_of(value_list<-1>{}, greater_one));
static_assert(all_of(value_list<'c'>{}, greater_one));
static_assert(!all_of(value_list<-1, 2u>{}, greater_one));
static_assert(!all_of(value_list<-1, 'c'>{}, greater_one));
static_assert(all_of(value_list<2u, 'c'>{}, greater_one));

// Check if a value exists inside the list with a certain condition.
//
static_assert(!any_of(value_list<>{}, greater_one));
static_assert(any_of(value_list<'c'>{}, greater_one));
static_assert(!any_of(value_list<-1>{}, greater_one));
static_assert(any_of(value_list<2u>{}, greater_one));
static_assert(!any_of(value_list<-2, -1>{}, greater_one));
static_assert(any_of(value_list<'c', -1>{}, greater_one));
static_assert(any_of(value_list<-1, 2u>{}, greater_one));

// Check that for each given element the predicate evaluates to false.
//
static_assert(none_of(value_list<>{}, greater_one));
static_assert(!none_of(value_list<'c'>{}, greater_one));
static_assert(none_of(value_list<-1>{}, greater_one));
static_assert(!none_of(value_list<2u>{}, greater_one));
static_assert(none_of(value_list<-2, -1>{}, greater_one));
static_assert(!none_of(value_list<'c', -1>{}, greater_one));
static_assert(!none_of(value_list<-1, 2u>{}, greater_one));

// Check if a 'value_list' instance contains a specific type.
//
static_assert(!contained(value_list<>{}, -1));
static_assert(!contained(value_list<>{}, 'c'));
static_assert(contained(value_list<-1>{}, -1));
static_assert(!contained(value_list<'c'>{}, -1));
static_assert(contained(value_list<-1, 'c'>{}, -1));
static_assert(contained(value_list<-1, 'c'>{}, 'c'));
static_assert(!contained(value_list<-1, 'c'>{}, 2u));

// Check whether the given value list is a set
//
static_assert(elementwise_unique(value_list<>{}));
static_assert(elementwise_unique(value_list<-1>{}));
static_assert(elementwise_unique(value_list<'c'>{}));
//
static_assert(elementwise_unique(value_list<-1, 'c'>{}));
static_assert(elementwise_unique(value_list<'c', -1>{}));
static_assert(!elementwise_unique(value_list<'c', 'c'>{}));
static_assert(!elementwise_unique(value_list<-1, -1>{}));

// Concatenate 'value_list' instances.
//
static_assert(concat(value_list<>{}, value_list<>{}) == value_list<>{});
static_assert(concat(value_list<-1>{}, value_list<>{}) == value_list<-1>{});
static_assert(concat(value_list<>{}, value_list<-1>{}) == value_list<-1>{});
static_assert(concat(value_list<-1>{}, value_list<'c'>{}) ==
              value_list<-1, 'c'>{});
static_assert(concat(value_list<'c'>{}, value_list<-1>{}) ==
              value_list<'c', -1>{});
static_assert(concat(value_list<'c', 1.23f>{}, value_list<-1>{}) ==
              value_list<'c', 1.23f, -1>{});
static_assert(concat(value_list<1.23f>{}, value_list<'c', -1>{}) ==
              value_list<1.23f, 'c', -1>{});
//
// You can also use the '+' operator for this.
//
static_assert(value_list<>{} + value_list<>{} == value_list<>{});
static_assert(value_list<-1>{} + value_list<>{} == value_list<-1>{});
static_assert(value_list<>{} + value_list<-1>{} == value_list<-1>{});
static_assert(value_list<-1>{} + value_list<'c'>{} == value_list<-1, 'c'>{});
static_assert(value_list<'c'>{} + value_list<-1>{} == value_list<'c', -1>{});
static_assert(value_list<'c', 1.23f>{} + value_list<-1>{} ==
              value_list<'c', 1.23f, -1>{});
static_assert(value_list<1.23f>{} + value_list<'c', -1>{} ==
              value_list<1.23f, 'c', -1>{});

// Add a value to the front of a 'value_list' instance.
//
static_assert(push_front<-1>(value_list<>{}) == value_list<-1>{});
static_assert(push_front<'c'>(value_list<-1>{}) == value_list<'c', -1>{});
static_assert(push_front<1.23f>(value_list<'c', -1>{}) ==
              value_list<1.23f, 'c', -1>{});
static_assert(push_front<value_list<>{}>(value_list<-1>{}) ==
              value_list<value_list<>{}, -1>{});

// Add a value to the back of a 'value_list' instance.
//
static_assert(push_back<-1>(value_list<>{}) == value_list<-1>{});
static_assert(push_back<'c'>(value_list<-1>{}) == value_list<-1, 'c'>{});
static_assert(push_back<1.23f>(value_list<'c', -1>{}) ==
              value_list<'c', -1, 1.23f>{});
static_assert(push_back<value_list<>{}>(value_list<-1>{}) ==
              value_list<-1, value_list<>{}>{});

// Pop an element from the front of a 'value_list' instance.
//
static_assert(pop_front(value_list<-1>{}) == value_list<>{});
static_assert(pop_front(value_list<'c', -1>{}) == value_list<-1>{});
static_assert(pop_front(value_list<1.23f, 'c', -1>{}) == value_list<'c', -1>{});
//
// You can also use '--' operator for this.
//
static_assert(--value_list<-1>{} == value_list<>{});
static_assert(--value_list<'c', -1>{} == value_list<-1>{});
static_assert(--value_list<1.23f, 'c', -1>{} == value_list<'c', -1>{});

// Pop an element from the back of a 'value_list' instance.
//
static_assert(pop_back(value_list<-1>{}) == value_list<>{});
static_assert(pop_back(value_list<'c', -1>{}) == value_list<'c'>{});
static_assert(pop_back(value_list<1.23f, 'c', -1>{}) ==
              value_list<1.23f, 'c'>{});
//
// You can also use the '--' operator for this.
//
static_assert((value_list<-1>{})-- == value_list<>{});
static_assert((value_list<'c', -1>{})-- == value_list<'c'>{});
static_assert((value_list<1.23f, 'c', -1>{})-- == value_list<1.23f, 'c'>{});

// Reverse the order of values in a 'value_list' instance.
//
static_assert(reverse(value_list<>{}) == value_list<>{});
static_assert(reverse(value_list<-1>{}) == value_list<-1>{});
static_assert(reverse(value_list<'c'>{}) == value_list<'c'>{});
static_assert(reverse(value_list<-1, 'c'>{}) == value_list<'c', -1>{});
static_assert(reverse(value_list<'c', -1>{}) == value_list<-1, 'c'>{});
static_assert(reverse(value_list<'c', -1, 1.23f>{}) ==
              value_list<1.23f, -1, 'c'>{});
//
// You can also use the '~' operator for this.
//
static_assert(~value_list<>{} == value_list<>{});
static_assert(~value_list<-1>{} == value_list<-1>{});
static_assert(~value_list<'c'>{} == value_list<'c'>{});
static_assert(~value_list<-1, 'c'>{} == value_list<'c', -1>{});
static_assert(~value_list<'c', -1>{} == value_list<-1, 'c'>{});
static_assert(~value_list<'c', -1, 1.23f>{} == value_list<1.23f, -1, 'c'>{});

// Trim the front.
//
static_assert(trim_front<0>(value_list<>{}) == value_list<>{});
static_assert(trim_front<0>(value_list<-1>{}) == value_list<-1>{});
static_assert(trim_front<1>(value_list<-1>{}) == value_list<>{});
static_assert(trim_front<0>(value_list<-1, 'c'>{}) == value_list<-1, 'c'>{});
static_assert(trim_front<1>(value_list<-1, 'c'>{}) == value_list<'c'>{});
static_assert(trim_front<2>(value_list<-1, 'c'>{}) == value_list<>{});
static_assert(trim_front<0>(value_list<-1, 'c', 1.23f>{}) ==
              value_list<-1, 'c', 1.23f>{});
static_assert(trim_front<1>(value_list<-1, 'c', 1.23f>{}) ==
              value_list<'c', 1.23f>{});
static_assert(trim_front<2>(value_list<-1, 'c', 1.23f>{}) ==
              value_list<1.23f>{});
static_assert(trim_front<3>(value_list<-1, 'c', 1.23f>{}) == value_list<>{});

// Trim the back.
//
static_assert(trim_back<0>(value_list<>{}) == value_list<>{});
static_assert(trim_back<0>(value_list<-1>{}) == value_list<-1>{});
static_assert(trim_back<1>(value_list<-1>{}) == value_list<>{});
static_assert(trim_back<0>(value_list<-1, 'c'>{}) == value_list<-1, 'c'>{});
static_assert(trim_back<1>(value_list<-1, 'c'>{}) == value_list<-1>{});
static_assert(trim_back<2>(value_list<-1, 'c'>{}) == value_list<>{});
static_assert(trim_back<0>(value_list<-1, 'c', 1.23f>{}) ==
              value_list<-1, 'c', 1.23f>{});
static_assert(trim_back<1>(value_list<-1, 'c', 1.23f>{}) ==
              value_list<-1, 'c'>{});
static_assert(trim_back<2>(value_list<-1, 'c', 1.23f>{}) == value_list<-1>{});
static_assert(trim_back<3>(value_list<-1, 'c', 1.23f>{}) == value_list<>{});

// Get a range of values from a given 'value_list' instance.
//
static_assert(range<0, 0>(value_list<>{}) == value_list<>{});
static_assert(range<0, 0>(value_list<-1>{}) == value_list<>{});
static_assert(range<1, 1>(value_list<-1>{}) == value_list<>{});
static_assert(range<0, 1>(value_list<-1>{}) == value_list<-1>{});
static_assert(range<0, 0>(value_list<-1, 'c'>{}) == value_list<>{});
static_assert(range<0, 1>(value_list<-1, 'c'>{}) == value_list<-1>{});
static_assert(range<0, 2>(value_list<-1, 'c'>{}) == value_list<-1, 'c'>{});
static_assert(range<1, 2>(value_list<-1, 'c'>{}) == value_list<'c'>{});
static_assert(range<2, 2>(value_list<-1, 'c'>{}) == value_list<>{});
static_assert(range<0, 0>(value_list<-1, 'c', 1.23f>{}) == value_list<>{});
static_assert(range<0, 1>(value_list<-1, 'c', 1.23f>{}) == value_list<-1>{});
static_assert(range<0, 2>(value_list<-1, 'c', 1.23f>{}) ==
              value_list<-1, 'c'>{});
static_assert(range<0, 3>(value_list<-1, 'c', 1.23f>{}) ==
              value_list<-1, 'c', 1.23f>{});
static_assert(range<0, 1>(value_list<-1, 'c', 1.23f>{}) == value_list<-1>{});
static_assert(range<1, 2>(value_list<-1, 'c', 1.23f>{}) == value_list<'c'>{});
static_assert(range<2, 3>(value_list<-1, 'c', 1.23f>{}) == value_list<1.23f>{});
static_assert(range<0, 2>(value_list<-1, 'c', 1.23f>{}) ==
              value_list<-1, 'c'>{});
static_assert(range<1, 3>(value_list<-1, 'c', 1.23f>{}) ==
              value_list<'c', 1.23f>{});

// Insert values at a specific position in a 'value_list' instance.
//
static_assert(insert<0, -1>(value_list<>{}) == value_list<-1>{});
static_assert(insert<0, 'c'>(value_list<-1>{}) == value_list<'c', -1>{});
static_assert(insert<1, 'c'>(value_list<-1>{}) == value_list<-1, 'c'>{});
static_assert(insert<0, 1.23f>(value_list<-1, 'c'>{}) ==
              value_list<1.23f, -1, 'c'>{});
static_assert(insert<1, 1.23f>(value_list<-1, 'c'>{}) ==
              value_list<-1, 1.23f, 'c'>{});
static_assert(insert<2, 1.23f>(value_list<-1, 'c'>{}) ==
              value_list<-1, 'c', 1.23f>{});

// Insert values in a 'value_list' instance by using a predicate.
//
constexpr auto less = []<auto x, auto y> { return x < y; };
static_assert(insert<-1>(value_list<>{}, less) == value_list<-1>{});
static_assert(insert<'c'>(value_list<>{}, less) == value_list<'c'>{});
static_assert(insert<-1>(value_list<'c'>{}, less) == value_list<-1, 'c'>{});
static_assert(insert<'d'>(value_list<-1, 'c'>{}, less) ==
              value_list<-1, 'c', 'd'>{});
static_assert(insert<0>(value_list<'c', -1>{}, less) ==
              value_list<0, 'c', -1>{});
static_assert(insert<3.21e-2>(value_list<-1, 'c'>{}, less) ==
              value_list<-1, 3.21e-2, 'c'>{});

// Get index of a unique type.
//
static_assert(index<-1>(value_list<-1>{}) == 0);
static_assert(index<-1>(value_list<'c', -1>{}) == 1);

// Remove values at a specific position in a 'value_list' instance.
//
static_assert(remove<0>(value_list<-1>{}) == value_list<>{});
static_assert(remove<0>(value_list<-1, 'c'>{}) == value_list<'c'>{});
static_assert(remove<1>(value_list<-1, 'c'>{}) == value_list<-1>{});
static_assert(remove<0>(value_list<1.23f, -1, 'c'>{}) == value_list<-1, 'c'>{});
static_assert(remove<1>(value_list<1.23f, -1, 'c'>{}) ==
              value_list<1.23f, 'c'>{});
static_assert(remove<2>(value_list<1.23f, -1, 'c'>{}) ==
              value_list<1.23f, -1>{});

// Remove values in a 'value_list' instance by using a predicate.
//
constexpr auto too_big = []<auto x> { return sizeof(x) > 2; };
static_assert(remove_all(value_list<>{}, too_big) == value_list<>{});
static_assert(remove_all(value_list<'c'>{}, too_big) == value_list<'c'>{});
static_assert(remove_all(value_list<-1>{}, too_big) == value_list<>{});
static_assert(remove_all(value_list<'c', -1>{}, too_big) == value_list<'c'>{});
static_assert(remove_all(value_list<-1, 'c'>{}, too_big) == value_list<'c'>{});
static_assert(remove_all(value_list<-1, 2u>{}, too_big) == value_list<>{});
static_assert(remove_all(value_list<'d', 'c'>{}, too_big) ==
              value_list<'d', 'c'>{});
static_assert(remove_all(value_list<-1, 2u, 'c', 'd'>{}, too_big) ==
              value_list<'c', 'd'>{});

// Swap two elements.
//
static_assert(swap<0, 0>(value_list<-1>{}) == value_list<-1>{});
//
static_assert(swap<0, 0>(value_list<-1, 'c'>{}) == value_list<-1, 'c'>{});
static_assert(swap<0, 1>(value_list<-1, 'c'>{}) == value_list<'c', -1>{});
static_assert(swap<1, 0>(value_list<-1, 'c'>{}) == value_list<'c', -1>{});
static_assert(swap<1, 1>(value_list<-1, 'c'>{}) == value_list<-1, 'c'>{});
//
static_assert(swap<0, 0>(value_list<-1, 'c', 1.23f>{}) ==
              value_list<-1, 'c', 1.23f>{});
static_assert(swap<0, 1>(value_list<-1, 'c', 1.23f>{}) ==
              value_list<'c', -1, 1.23f>{});
static_assert(swap<0, 2>(value_list<-1, 'c', 1.23f>{}) ==
              value_list<1.23f, 'c', -1>{});
static_assert(swap<1, 0>(value_list<-1, 'c', 1.23f>{}) ==
              value_list<'c', -1, 1.23f>{});
static_assert(swap<1, 1>(value_list<-1, 'c', 1.23f>{}) ==
              value_list<-1, 'c', 1.23f>{});
static_assert(swap<1, 2>(value_list<-1, 'c', 1.23f>{}) ==
              value_list<-1, 1.23f, 'c'>{});
static_assert(swap<2, 0>(value_list<-1, 'c', 1.23f>{}) ==
              value_list<1.23f, 'c', -1>{});
static_assert(swap<2, 1>(value_list<-1, 'c', 1.23f>{}) ==
              value_list<-1, 1.23f, 'c'>{});
static_assert(swap<2, 2>(value_list<-1, 'c', 1.23f>{}) ==
              value_list<-1, 'c', 1.23f>{});

constexpr auto less_mem = []<auto x, auto y> { return sizeof(x) <= sizeof(y); };

// Merge two sorted 'value_list' instances.
//
static_assert(merge(value_list<>{}, value_list<>{}, less_mem) ==
              value_list<>{});
//
static_assert(merge(value_list<>{}, value_list<-1>{}, less_mem) ==
              value_list<-1>{});
static_assert(merge(value_list<-1>{}, value_list<>{}, less_mem) ==
              value_list<-1>{});
static_assert(merge(value_list<>{}, value_list<'c', -1>{}, less_mem) ==
              value_list<'c', -1>{});
static_assert(merge(value_list<'c', -1>{}, value_list<>{}, less_mem) ==
              value_list<'c', -1>{});
//
static_assert(merge(value_list<'c'>{}, value_list<-1>{}, less_mem) ==
              value_list<'c', -1>{});
static_assert(merge(value_list<-1>{}, value_list<'c'>{}, less_mem) ==
              value_list<'c', -1>{});
//
static_assert(merge(value_list<'c', short{0}>{}, value_list<-1>{}, less_mem) ==
              value_list<'c', short{0}, -1>{});
static_assert(merge(value_list<'c'>{}, value_list<short{0}, -1>{}, less_mem) ==
              value_list<'c', short{0}, -1>{});
//
static_assert(merge(value_list<'c', short{0}>{},
                    value_list<-1, 3.21e-1>{},
                    less_mem) == value_list<'c', short{0}, -1, 3.21e-1>{});
static_assert(merge(value_list<'c', -1>{},
                    value_list<short{0}, 3.21e-1>{},
                    less_mem) == value_list<'c', short{0}, -1, 3.21e-1>{});
static_assert(merge(value_list<'c', 3.21e-1>{},
                    value_list<short{0}, -1>{},
                    less_mem) == value_list<'c', short{0}, -1, 3.21e-1>{});
static_assert(merge(value_list<short{0}, -1>{},
                    value_list<'c', 3.21e-1>{},
                    less_mem) == value_list<'c', short{0}, -1, 3.21e-1>{});
//
static_assert(merge(value_list<'c'>{},
                    value_list<short{0}, -1, 3.21e-1>{},
                    less_mem) == value_list<'c', short{0}, -1, 3.21e-1>{});
static_assert(merge(value_list<short{0}>{},
                    value_list<'c', -1, 3.21e-1>{},
                    less_mem) == value_list<'c', short{0}, -1, 3.21e-1>{});
static_assert(merge(value_list<-1>{},
                    value_list<'c', short{0}, 3.21e-1>{},
                    less_mem) == value_list<'c', short{0}, -1, 3.21e-1>{});
static_assert(merge(value_list<3.21e-1>{},
                    value_list<'c', short{0}, -1>{},
                    less_mem) == value_list<'c', short{0}, -1, 3.21e-1>{});

// Sort a list of values.
//
static_assert(sort(value_list<>{}, less_mem) == value_list<>{});
//
static_assert(sort(value_list<-1>{}, less_mem) == value_list<-1>{});
static_assert(sort(value_list<'c'>{}, less_mem) == value_list<'c'>{});
//
static_assert(sort(value_list<'c', -1>{}, less_mem) == value_list<'c', -1>{});
static_assert(sort(value_list<-1, 'c'>{}, less_mem) == value_list<'c', -1>{});
static_assert(sort(value_list<unsigned{1}, -1>{}, less_mem) ==
              value_list<unsigned{1}, -1>{});
static_assert(sort(value_list<-1, unsigned{1}>{}, less_mem) ==
              value_list<-1, unsigned{1}>{});
//
static_assert(sort(value_list<'c', short{0}, -1>{}, less_mem) ==
              value_list<'c', short{0}, -1>{});
static_assert(sort(value_list<'c', -1, short{0}>{}, less_mem) ==
              value_list<'c', short{0}, -1>{});
static_assert(sort(value_list<short{0}, 'c', -1>{}, less_mem) ==
              value_list<'c', short{0}, -1>{});
static_assert(sort(value_list<short{0}, -1, 'c'>{}, less_mem) ==
              value_list<'c', short{0}, -1>{});
static_assert(sort(value_list<-1, 'c', short{0}>{}, less_mem) ==
              value_list<'c', short{0}, -1>{});
static_assert(sort(value_list<-1, short{0}, 'c'>{}, less_mem) ==
              value_list<'c', short{0}, -1>{});
//
static_assert(sort(value_list<short{0}, unsigned{1}, 'c', -1>{},
                   []<auto x, auto y> { return sizeof(x) >= sizeof(y); }) ==
              value_list<unsigned{1}, -1, short{0}, 'c'>{});
static_assert(sort(value_list<short{0}, unsigned{1}, 'c', -1>{},
                   []<auto x, auto y> { return sizeof(x) > sizeof(y); }) ==
              value_list<-1, unsigned{1}, short{0}, 'c'>{});

// Apply transform algorithm to instance of `value_list`.
//
constexpr auto increment = []<auto x> -> decltype(x) { return x + 1; };
//
static_assert(transform(value_list<>{}, increment) == value_list<>{});
//
static_assert(transform(value_list<-1>{}, increment) == value_list<0>{});
static_assert(transform(value_list<'c'>{}, increment) == value_list<'d'>{});
static_assert(transform(value_list<1.23f>{}, increment) == value_list<2.23f>{});
//
static_assert(transform(value_list<-1, 'c'>{}, increment) ==
              value_list<0, 'd'>{});
static_assert(transform(value_list<'c', 1.23f>{}, increment) ==
              value_list<'d', 2.23f>{});
static_assert(transform(value_list<1.23f, -1>{}, increment) ==
              value_list<2.23f, 0>{});
//
static_assert(transform(value_list<-1, 'c', 1.23f, 3.21e-1>{},
                        []<auto x> { return as_value<decltype(x)>; }) ==
              value_list<type_tag<int>{},
                         type_tag<char>{},
                         type_tag<float>{},
                         type_tag<double>{}>{});

// Fold all values together.
//
static_assert(fold(value_list<-1, 'c', 1.23f, 3.21e-1>{},
                   []<auto... values> {
                     return as_value<type_list<as_type<values>...>>;
                   }) == type_list<value_tag<-1>,
                                   value_tag<'c'>,
                                   value_tag<1.23f>,
                                   value_tag<3.21e-1>>{});

int main() {
  constexpr auto matcher = match{
      [](int x) { return std::format("{}: int", x); },
      [](char x) { return std::format("{}: char", x); },
      [](float x) { return std::format("{}: float", x); },
      [](double x) { return std::format("{}: double", x); },
  };

  constexpr auto values = value_list<-1, 'c', 1.23f, 3.21e-1>{};

  int index = 0;
  for_each(values, [&]<auto value> {
    std::print("value[{}] = {}\n", index++, matcher(value));
  });

  index = 0;

  const auto succeded = for_each_until(values, [&]<auto value> {
    std::print("value[{}] = {}\n", index++, matcher(value));
    return as_value<decltype(value)> == as_value<float>;
  });

  if (succeded) std::print("Finished before end of list.\n");
}
