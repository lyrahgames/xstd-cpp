#include <doctest/doctest.h>
//
#include <xstd/concurrency.hpp>

SCENARIO("xstd::task_bind") {
  {
    auto f = xstd::task_bind([]() { return 0; });
    CHECK(f() == 0);
  }
  {
    auto f = xstd::task_bind([](int x) { return x; }, 0);
    CHECK(f() == 0);
  }
  {
    auto f = xstd::task_bind([](int x, auto y) { return x + y; }, 0, 1);
    CHECK(f() == 1);
  }

  {
    auto f = xstd::task_bind<int>([](int x) { return x; });
    CHECK(f(1) == 1);
  }
  {
    auto f = xstd::task_bind<int>([](int x, auto y) { return x + y; }, -1);
    CHECK(f(1) == 0);
  }

  {
    auto f = xstd::task_bind<int, int>([](int x, int y) { return x + y; });
    CHECK(f(1, -1) == 0);
  }
  {
    auto f = xstd::task_bind<int, int>(
        [](int x, int y, int z) { return x + y + z; }, -1);
    CHECK(f(1, -1) == -1);
  }
}

SCENARIO("xstd::task_bind_r") {
  {
    auto f = xstd::task_bind_r<size_t>([]() { return 0; });
    CHECK(f() == 0);
  }
  {
    auto f = xstd::task_bind_r<size_t>([](int x) { return x; }, 0);
    CHECK(f() == 0);
  }
  {
    auto f =
        xstd::task_bind_r<size_t>([](int x, auto y) { return x + y; }, 0, 1);
    CHECK(f() == 1);
  }

  {
    auto f = xstd::task_bind_r<size_t, int>([](int x) { return x; });
    CHECK(f(1) == 1);
  }
  {
    auto f =
        xstd::task_bind_r<size_t, int>([](int x, auto y) { return x + y; }, -1);
    CHECK(f(1) == 0);
  }

  {
    auto f =
        xstd::task_bind_r<size_t, int, int>([](int x, int y) { return x + y; });
    CHECK(f(1, -1) == 0);
  }
  {
    auto f = xstd::task_bind_r<size_t, int, int>(
        [](int x, int y, int z) { return x + y + z; }, -1);
    CHECK(f(1, -1) == -1);
  }
}

SCENARIO("xstd::basic_task_queue") {
  {
    std::array<bool, 10> data{};
    CHECK(std::all_of(data.begin(), data.end(), [](bool x) { return !x; }));
    xstd::basic_task_queue<> tasks{};
    auto setter = [&](int index) { data[index] = true; };
    for (size_t i = 0; i < data.size(); ++i)
      tasks.async_invoke_and_discard(setter, i);
    std::jthread thread1{
        [&](std::stop_token stop_token) { tasks.process_all(); }};
    std::jthread thread2{
        [&](std::stop_token stop_token) { tasks.process_all(); }};
    thread1.join();
    thread2.join();
    CHECK(std::all_of(data.begin(), data.end(), [](bool x) { return x; }));
  }
  {
    using data_type = std::array<bool, 10>;
    using task_queue = xstd::basic_task_queue<data_type&>;
    task_queue tasks{};
    auto setter = [&](data_type& data, int index) { data[index] = true; };
    for (size_t i = 0; i < data_type{}.size(); ++i)
      tasks.async_invoke_and_discard(setter, i);
    std::jthread thread{[&tasks] {
      data_type data{};
      CHECK(std::all_of(data.begin(), data.end(), [](bool x) { return !x; }));
      tasks.process_all(data);
      CHECK(std::all_of(data.begin(), data.end(), [](bool x) { return x; }));
    }};
    thread.join();
  }
}
