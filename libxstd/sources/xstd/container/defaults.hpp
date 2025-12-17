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
#include <xstd/ranges/defaults.hpp>
//

// Sequence Container
//
#include <deque>
#include <forward_list>
#include <list>
#include <vector>

// Ordered Associative Container
//
#include <map>
#include <set>

// Unordered Associative Container
//
#include <unordered_map>
#include <unordered_set>

// Container Adaptors
//
#include <queue>
#include <stack>
#if __has_include(<flat_map>)
#include <flat_map>
#endif
#if __has_include(<flat_set>)
#include <flat_set>
#endif

namespace xstd {}  // namespace xstd
