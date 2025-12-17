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
#include <xstd/concurrency/task_queue.hpp>

namespace xstd {

class task_thread {
 public:
  task_thread() noexcept
      : thread{[this](std::stop_token stop_token) { tasks.run(stop_token); }} {}

  auto get_id() const noexcept -> std::jthread::id { return thread.get_id(); }

  void join() { thread.join(); }

  auto get_stop_source() noexcept -> std::stop_source {
    return thread.get_stop_source();
  }

  auto get_stop_token() const noexcept -> std::stop_token {
    return thread.get_stop_token();
  }

  bool request_stop() noexcept { return thread.request_stop(); }

  /// Asynchronously invoke the callable `f` with arguments
  /// `args...` on the task thread in fire-and-forget style.
  /// The function neither blocks nor returns anything.
  ///
  void async_invoke_and_discard(auto&& f, auto&&... args) {
    tasks.async_invoke_and_discard(std::forward<decltype(f)>(f),
                                   std::forward<decltype(args)>(args)...);
  }

  /// Asynchronously invoke `f` with arguments `args...` on the task thread.
  /// The function returns an `std::future` that will contain the return value.
  ///
  [[nodiscard]] auto async_invoke(auto&& f, auto&&... args) {
    return tasks.async_invoke(std::forward<decltype(f)>(f),
                              std::forward<decltype(args)>(args)...);
  }

  /// Asynchronously invoke the callable `f` with arguments `args...` on
  /// the task thread and implicitly convert its return value to `result`.
  /// The function returns an `std::future` that will contain the return value.
  ///
  template <typename result>
  [[nodiscard]] auto async_invoke(auto&& f, auto&&... args) {
    return tasks.template async_invoke<result>(
        std::forward<decltype(f)>(f), std::forward<decltype(args)>(args)...);
  }

  /// Synchronously invoke the callable `f` with
  /// arguments `args...` on the task thread.
  /// If the function is already called on the main thread, it simply
  /// forwards to `std::invoke` to prevent indefinite blocking.
  ///
  auto invoke(auto&& f, auto&&... args) {
    // Forward to `std::invoke` when called on task thread.
    if (get_id() == std::this_thread::get_id())
      return std::invoke(std::forward<decltype(f)>(f),
                         std::forward<decltype(args)>(args)...);
    // Otherwise, enqueue callable as task for asynchronous invocation.
    // Wait for the retrieved `std::future` to be available.
    auto task = async_invoke(std::forward<decltype(f)>(f),
                             std::forward<decltype(args)>(args)...);
    return task.get();
  }

  /// Synchronously invoke the callable `f` with arguments `args...` on
  /// the task thread and implicitly convert its return value to `result`.
  /// If the function is already called on the main thread, it simply
  /// forwards to `std::invoke_r` to prevent indefinite blocking.
  ///
  template <typename result>
  auto invoke(auto&& f, auto&&... args) {
    // Forward to `std::invoke_r` when called on task thread.
    if (get_id() == std::this_thread::get_id())
      return std::invoke_r<result>(std::forward<decltype(f)>(f),
                                   std::forward<decltype(args)>(args)...);
    // Otherwise, enqueue callable as task for asynchronous invocation.
    // Wait for the retrieved `std::future` to be available.
    auto task = async_invoke<result>(std::forward<decltype(f)>(f),
                                     std::forward<decltype(args)>(args)...);
    return task.get();
  }

 private:
  task_queue tasks{};
  std::jthread thread{};
};

}  // namespace xstd
