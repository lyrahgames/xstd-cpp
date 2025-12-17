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
//
#include <atomic>
#include <barrier>
#include <condition_variable>
#include <future>
#include <latch>
#include <mutex>
#include <semaphore>
#include <shared_mutex>
#include <thread>

namespace xstd {

/// `async_invoke` acts like `std::async` but uses
/// `std::launch::async` as launch policy.
///
/// "[...] having a function that acts like `std::async`,
/// but that automatically uses `std::launch::async` as the
/// launch policy, is a convenient tool to have around, [...]"
///
/// from Effective Modern C++ by Scott Meyers
///
[[nodiscard]] inline auto async_invoke(auto&& f, auto&&... args) {
  return std::async(std::launch::async,  //
                    std::forward<decltype(f)>(f),
                    std::forward<decltype(args)>(args)...);
}

/// Checks whether the given type can be used as a task object,
/// i.e., is movable and invocable with no arguments.
///
template <typename type>
concept nullary_task = std::invocable<type>;

/// Checks whether the given type is a nullary task and its return type
/// from its invocation with no arguments coincides with the given return type.
///
template <typename type, typename result>
concept nullary_task_for =
    nullary_task<type> && std::same_as<result, std::invoke_result_t<type>>;

/// Bind the callable `f` to the arguments `args...` for postponed invocation.
/// The returned callable is invocable for `params...` and, intrinsically,
/// calls `std::invoke` to invoke `f` with instantiated `params...` and `args`.
///
template <typename... params, typename... bindings>
[[nodiscard]] constexpr auto task_bind(
    std::invocable<params..., bindings...> auto&& f,
    bindings&&... args) -> std::invocable<params...> auto {
  // Return a lambda expression that captures `f` and `args` by decay-copy.
  // This is important for reference-like function arguments that might
  // have already been outlived when function execution is postponed.
  return [f = auto(std::forward<decltype(f)>(f)),
          ... args = auto(std::forward<bindings>(args))]
      // For proper forwarding, the lambda's parameters include the self reference.
      // All other parameters are given by the params directly.
      (this auto&& self, params&&... p) {
        // The implementation only uses `std::invoke` to
        // invoke the function with all given arguments.
        return std::invoke(std::forward_like<decltype(self)>(f),
                           std::forward<params>(p)...,
                           std::forward_like<decltype(self)>(args)...);
      };
}

/// Bind the callable `f` to the arguments `args...` for postponed invocation.
/// The returned callable is invocable for `params...` and, intrinsically,
/// calls `std::invoke_r` to invoke `f` with instantiated `params...` and `args`.
/// The return value is implicitly converted to `result` type.
///
template <typename result, typename... params, typename... bindings>
[[nodiscard]] constexpr auto task_bind_r(
    xstd::invocable_r<result, params..., bindings...> auto&& f,
    bindings&&... args) -> xstd::strict_invocable_r<result, params...> auto {
  // Return a lambda expression that decay-copy captures `f` and `args`.
  // This is important for postponed execution of the function
  // when function arguments might already be outlived.
  return [f = auto(std::forward<decltype(f)>(f)),
          ... args = auto(std::forward<bindings>(args))]
      // For proper forwarding, the lambda's parameters include the self reference.
      // All other parameters are given by the params directly.
      (this auto&& self, params&&... p) {
        // The implementation only uses `std::invoke_r` to invoke the function
        // with all given arguments and implicitly convert it to `result`.
        return std::invoke_r<result>(
            std::forward_like<decltype(self)>(f), std::forward<params>(p)...,
            std::forward_like<decltype(self)>(args)...);
      };
}

}  // namespace xstd
