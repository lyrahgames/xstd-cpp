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
#include <xstd/concurrency/defaults.hpp>
//
#include <queue>

namespace xstd {

/// The `task_queue` class is a thread-safe queue of tasks.
/// Multiple threads are allowed to push new tasks to the queue.
/// Multiple threads are allowed to process tasks from the queue.
/// All given tasks are either seen as fire-and-forget (`push_and_discard`)
/// tasks without any return value or packaged by `std::packaged_task`
/// that returns its respective `std::future` to allow for synchronization.
///
class task_queue {
 public:
  /// The specific task type that is used to store tasks in the queue.
  ///
  using task_type = std::move_only_function<void()>;

  /// The queue container type used to store all tasks.
  /// The `std::queue` container adaptor uses `std::deque` be default.
  ///
  using queue_type = std::queue<task_type>;

  /// Default Constructor
  ///
  task_queue() noexcept = default;

  /// Copy construction and assignment is forbidden.
  ///
  task_queue(const task_queue&) = delete;
  task_queue& operator=(const task_queue&) = delete;

  /// Move Constructor
  ///
  task_queue(task_queue&& other) noexcept {
    // Use a scope to unblock before notifying `other`.
    {
      std::scoped_lock lock{other.mutex};
      tasks.swap(other.tasks);
    }
    // As we are only constructing the object,
    // only `other` needs to be notified.
    other.condition.notify_all();
  }

  /// Move Assignment
  ///
  task_queue& operator=(task_queue&& other) noexcept {
    // Use a scope to unblock before notifying `this` and `other`.
    {
      std::scoped_lock lock{mutex, other.mutex};
      tasks.swap(other.tasks);
    }
    // The contents of both, `this` and `other`, might have changed drastically.
    // Thus, we notify all waiting threads at once to allow for reschedule.
    condition.notify_all();
    other.condition.notify_all();
    return *this;
  }

  /// Push a fire-and-forget task with no return value to the queue.
  /// This is a primitive used to implement other enqueuing operations.
  ///
  void push_and_discard(nullary_task_for<void> auto&& task) {
    // Use a scope to unblock before notifying a waiting thread.
    {
      std::scoped_lock lock{mutex};
      tasks.emplace(std::forward<decltype(task)>(task));
    }
    // In this case, only a single thread needs to be notified
    // as only one new task was pushed to the queue.
    condition.notify_one();
  }

  /// Push a fire-and-forget task with return value to the queue.
  /// The return value received by invoking the callable `f` will be discarded.
  /// This is a primitive used to implement other enqueuing operations.
  ///
  void push_and_discard(nullary_task auto&& f) {
    push_and_discard([task = std::forward<decltype(f)>(f)](this auto&& self) {
      // Discard the return value.
      // The compiler should warn about `nodiscard` functions.
      std::invoke(std::forward_like<decltype(self)>(task));
    });
  }

  /// Push an arbitrary task to the queue and receive a `std::future`
  /// to its wrapping `std::package_task` for synchronization.
  /// Thus, the result is not allowed to be discarded.
  /// This is a primitive used to implement other enqueuing operations.
  ///
  template <nullary_task functor>
  [[nodiscard]] auto push(functor&& f) {
    // CTAD of `std::packaged_task` may fail for
    // more complicated callable types.
    // Thus, we use an explicit template argument.
    using result_type = std::invoke_result_t<functor>;
    std::packaged_task<result_type()> task{std::forward<decltype(f)>(f)};
    auto result = task.get_future();
    push_and_discard(std::move(task));
    return result;
  }

  /// Enqueue a fire-and-forget task constructed by
  /// binding the callable `f` to the arguments `args...`.
  /// The return value is discarded by implicit conversion to type `void`.
  /// If at least one thread processes the `task_queue` object,
  /// this function can be understood to asynchronously invoke
  /// the constructed task without synchronization capabilities.
  /// The overload for no given arguments directly forwards to `push_and_discard`.
  ///
  void async_invoke_and_discard(auto&& f, auto&&... args) {
    push_and_discard(std::bind<void>(std::forward<decltype(f)>(f),
                                     std::forward<decltype(args)>(args)...));
  }
  //
  void async_invoke_and_discard(nullary_task auto&& f) {
    push_and_discard(std::forward<decltype(f)>(f));
  }

  /// Enqueue a task constructed by binding the callable `f` to the arguments
  /// `args...` and receive its respective `std::future` for synchronization.
  /// If at least one thread processes the `task_queue` object,
  /// this function can be understood to asynchronously invoke
  /// the constructed task with synchronization capabilities.
  /// The overload for no given arguments directly forwards to `push`.
  ///
  [[nodiscard]] auto async_invoke(auto&& f, auto&&... args) {
    return push(std::bind(std::forward<decltype(f)>(f),
                          std::forward<decltype(args)>(args)...));
  }
  //
  [[nodiscard]] auto async_invoke(nullary_task auto&& f) {
    return push(std::forward<decltype(f)>(f));
  }

  /// Enqueue a task constructed by binding the callable `f` to the arguments
  /// `args...` and receive its respective `std::future` for synchronization.
  /// The return value will be implicitly converted to `result` type.
  /// If at least one thread processes the `task_queue` object,
  /// this function can be understood to asynchronously invoke
  /// the constructed task with synchronization capabilities.
  ///
  template <typename result>
  [[nodiscard]] auto async_invoke(auto&& f, auto&&... args) {
    return push(std::bind<result>(std::forward<decltype(f)>(f),
                                  std::forward<decltype(args)>(args)...));
  }

  /// Synchronously invoke the callable `f` with arguments `args...`.
  /// This call blocks the calling thread until the invocation returns.
  /// This function will implicitly construct an `std::packaged_task`
  /// by binding `f` and `args...` and receive its respective `std::future`.
  /// The task will only be enqueued and must be processed by a different
  /// thread using the `process` primitive to prevent indefinite blocking.
  /// If a `task_queue` is only processed by a specific thread,
  /// this routine can be used to make sure that a given callable
  /// is only invoked on that specific thread as it may be the case for GUIs.
  ///
  auto invoke(auto&& f, auto&&... args) {
    return async_invoke(std::forward<decltype(f)>(f),
                        std::forward<decltype(args)>(args)...)
        .get();
  }

  /// Return `false` if the queue is empty.
  /// Otherwise, pop the next task from the queue,
  /// invoke it on the current thread, and return `true`.
  ///
  bool process() {
    task_type task{};
    {
      std::scoped_lock lock{mutex};
      if (tasks.empty()) return false;
      task = move(tasks.front());
      tasks.pop();
    }
    std::invoke(std::move(task));
    return true;
  }

  /// Process all available tasks in the queue on the current thread.
  ///
  void process_all() { while (process()); }

  /// Wait until the `task_queue` object is not empty anymore
  /// and process the next waiting task in the queue.
  /// The waiting can be interrupted by using a `std::stop_source`
  /// that provided an instance of `std::stop_token` as argument.
  /// This function blocks the current thread until a stop signal
  /// has been received or a task could be processed.
  /// The function returns `true` if a task was processed.
  /// It returns `false` if a stop request made it stop.
  ///
  bool wait_and_process(std::stop_token stop_token) {
    task_type task{};
    {
      std::unique_lock lock{mutex};
      if (!condition.wait(lock, stop_token,
                          [this] { return not tasks.empty(); }))
        return false;
      task = std::move(tasks.front());
      tasks.pop();
    }
    std::invoke(std::move(task));
    return true;
  }

  /// Continuously wait for tasks in the queue and process them.
  /// This function will block the current thread and may only be
  /// interrupted by the use of an `std::stop_source` that provided
  /// a respective `std::stop_token` as argument.
  ///
  void run(std::stop_token stop_token) { while (wait_and_process(stop_token)); }

 private:
  // Data Members
  //
  queue_type tasks{};          // Queue that contains all tasks.
  mutable std::mutex mutex{};  // Mutual exclusion for thread-safety.
  mutable std::condition_variable_any
      condition{};  // Condition variable to check for emptiness.
};

}  // namespace xstd
