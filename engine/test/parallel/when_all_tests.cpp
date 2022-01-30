#include "genebits/engine/parallel/when_all.h"

#include <algorithm>
#include <thread>

#include <gmock/gmock.h>

#include "genebits/engine/parallel/sync_wait.h"

namespace genebits::engine
{
namespace
{
  struct AsyncExecuteAwaiter
  {
    std::thread* thread;

    bool await_ready() const noexcept
    {
      return false;
    }

    void await_resume() const noexcept {}

    void await_suspend(std::coroutine_handle<> coro) noexcept
    {
      new (thread) std::thread([](std::coroutine_handle<> handle) { handle.resume(); }, coro);
    }
  };
} // namespace

TEST(WhenAllTask_Tests, WhenAllReady_None_DoNothing)
{
  std::atomic_size_t count = 0;

  std::thread thread;

  auto task = [&]() -> Task<>
  {
    co_await WhenAllReady();
    count++;
  }();

  task.Eject();

  task.Poll();

  EXPECT_TRUE(task.IsReady());
  EXPECT_EQ(count, 1);
}

TEST(WhenAllTask_Tests, WhenAllReady_Single_Execute)
{
  std::atomic_size_t count = 0;

  std::thread thread;

  auto make_task = [&]() -> Task<>
  {
    co_await AsyncExecuteAwaiter { &thread };

    std::this_thread::sleep_for(std::chrono::milliseconds { 1 });

    count++;
  };

  auto task = [&]() -> Task<> { co_await WhenAllReady(make_task()); }();

  SyncWait(task);

  EXPECT_TRUE(task.IsReady());
  EXPECT_EQ(count, 1);

  if (thread.joinable()) thread.join();
}

TEST(WhenAllTask_Tests, WhenAllReady_SingleWithResult_ExecuteAndIgnoreResult)
{
  std::atomic_size_t count = 0;

  std::thread thread;

  auto make_task = [&]() -> Task<int>
  {
    co_await AsyncExecuteAwaiter { &thread };

    std::this_thread::sleep_for(std::chrono::milliseconds { 1 });

    count++;

    co_return 10;
  };

  auto task = [&]() -> Task<> { co_await WhenAllReady(make_task()); }();

  SyncWait(task);

  EXPECT_TRUE(task.IsReady());
  EXPECT_EQ(count, 1);

  if (thread.joinable()) thread.join();
}

TEST(WhenAllTask_Tests, WhenAllReady_Double_Execute)
{
  std::atomic_size_t count = 0;

  std::thread thread1;
  std::thread thread2;

  auto make_task = [&](std::thread* thread) -> Task<>
  {
    co_await AsyncExecuteAwaiter { thread };

    std::this_thread::sleep_for(std::chrono::milliseconds { 1 });

    count++;
  };

  auto task = [&]() -> Task<> { co_await WhenAllReady(make_task(&thread1), make_task(&thread2)); }();

  SyncWait(task);

  EXPECT_TRUE(task.IsReady());
  EXPECT_EQ(count, 2);

  if (thread1.joinable()) thread1.join();
  if (thread2.joinable()) thread2.join();
}

TEST(WhenAllTask_Tests, WhenAllReady_Many_Execute)
{
  std::atomic_size_t count = 0;

  std::thread thread[4];

  auto make_task = [&](std::thread* thread) -> Task<>
  {
    co_await AsyncExecuteAwaiter { thread };

    std::this_thread::sleep_for(std::chrono::milliseconds { 1 });

    count++;
  };

  auto task = [&]() -> Task<> {
    co_await WhenAllReady(make_task(&thread[0]), make_task(&thread[1]), make_task(&thread[2]), make_task(&thread[3]));
  }();

  SyncWait(task);

  EXPECT_TRUE(task.IsReady());
  EXPECT_EQ(count, 4);

  for (size_t i = 0; i < 4; i++)
  {
    if (thread[i].joinable()) thread[i].join();
  }
}

TEST(WhenAllTask_Tests, WhenAllReady_ManyAsRange_Execute)
{
  std::atomic_size_t count = 0;

  static constexpr size_t amount = 100;

  std::thread thread[amount];

  auto make_task = [&](std::thread* thread) -> Task<>
  {
    co_await AsyncExecuteAwaiter { thread };

    std::this_thread::sleep_for(std::chrono::milliseconds { 1 });

    count++;
  };

  auto task = [&]() -> Task<>
  {
    FastVector<Task<>> tasks;

    for (size_t i = 0; i < amount; i++)
    {
      tasks.PushBack(make_task(&thread[i]));
    }

    co_await WhenAllReady(std::move(tasks));
  }();

  SyncWait(task);

  EXPECT_TRUE(task.IsReady());
  EXPECT_EQ(count, amount);

  for (size_t i = 0; i < amount; i++)
  {
    if (thread[i].joinable()) thread[i].join();
  }
}

TEST(WhenAllTask_Tests, WhenAllReady_ManyWithResultAsRange_Execute)
{
  std::atomic_size_t count = 0;

  static constexpr size_t amount = 100;

  std::thread thread[amount];

  auto make_task = [&](std::thread* thread) -> Task<int>
  {
    co_await AsyncExecuteAwaiter { thread };

    std::this_thread::sleep_for(std::chrono::milliseconds { 1 });

    count++;

    co_return 10;
  };

  auto task = [&]() -> Task<>
  {
    FastVector<Task<int>> tasks;

    for (size_t i = 0; i < amount; i++)
    {
      tasks.PushBack(make_task(&thread[i]));
    }

    co_await WhenAllReady(std::move(tasks));
  }();

  SyncWait(task);

  EXPECT_TRUE(task.IsReady());
  EXPECT_EQ(count, amount);

  for (size_t i = 0; i < amount; i++)
  {
    if (thread[i].joinable()) thread[i].join();
  }
}

TEST(WhenAllTask_Tests, WhenAll_None_DoNothing)
{
  std::atomic_size_t count = 0;

  std::thread thread;

  auto task = [&]() -> Task<>
  {
    co_await WhenAll();
    count++;
  }();

  task.Eject();

  task.Poll();

  EXPECT_TRUE(task.IsReady());
  EXPECT_EQ(count, 1);
}

TEST(WhenAllTask_Tests, WhenAll_SingleVoid_Execute)
{
  std::atomic_size_t count = 0;

  std::thread thread;

  auto make_task = [&]() -> Task<>
  {
    co_await AsyncExecuteAwaiter { &thread };

    std::this_thread::sleep_for(std::chrono::milliseconds { 1 });

    count++;
  };

  auto task = [&]() -> Task<> { co_await WhenAll(make_task()); }();

  SyncWait(task);

  EXPECT_TRUE(task.IsReady());
  EXPECT_EQ(count, 1);

  if (thread.joinable()) thread.join();
}

TEST(WhenAllTask_Tests, WhenAll_SingleResult_CorrectValue)
{
  std::atomic_size_t count = 0;

  std::thread thread;

  auto make_task = [&]() -> Task<int>
  {
    co_await AsyncExecuteAwaiter { &thread };

    std::this_thread::sleep_for(std::chrono::milliseconds { 1 });

    count++;

    co_return 10;
  };

  auto task = [&]() -> Task<int>
  {
    auto [value] = co_await WhenAll(make_task());

    co_return value;
  }();

  auto value = SyncWait(task);

  EXPECT_TRUE(task.IsReady());
  EXPECT_EQ(count, 1);
  EXPECT_EQ(value, 10);

  if (thread.joinable()) thread.join();
}

TEST(WhenAllTask_Tests, WhenAll_DoubleBothVoid_Executed)
{
  std::atomic_size_t count = 0;

  std::thread thread1;
  std::thread thread2;

  auto make_task = [&](std::thread* thread) -> Task<>
  {
    co_await AsyncExecuteAwaiter { thread };

    std::this_thread::sleep_for(std::chrono::milliseconds { 1 });

    count++;
  };

  auto task = [&]() -> Task<> { co_await WhenAll(make_task(&thread1), make_task(&thread2)); }();

  SyncWait(task);

  EXPECT_TRUE(task.IsReady());
  EXPECT_EQ(count, 2);

  if (thread1.joinable()) thread1.join();
  if (thread2.joinable()) thread2.join();
}

TEST(WhenAllTask_Tests, WhenAll_DoubleBothResults_CorrectValue)
{
  std::atomic_size_t count = 0;

  std::thread thread1;
  std::thread thread2;

  auto make_task = [&](std::thread* thread) -> Task<int>
  {
    co_await AsyncExecuteAwaiter { thread };

    std::this_thread::sleep_for(std::chrono::milliseconds { 1 });

    count++;

    co_return 10;
  };

  auto task = [&]() -> Task<int>
  {
    auto [value1, value2] = co_await WhenAll(make_task(&thread1), make_task(&thread2));

    co_return value1 + value2 / 2;
  }();

  auto value = SyncWait(task);

  EXPECT_TRUE(task.IsReady());
  EXPECT_EQ(count, 2);
  EXPECT_EQ(value, 15);

  if (thread1.joinable()) thread1.join();
  if (thread2.joinable()) thread2.join();
}

TEST(WhenAllTask_Tests, WhenAll_DoubleLeftVoid_CorrectValue)
{
  std::atomic_size_t count = 0;

  std::thread thread1;
  std::thread thread2;

  auto make_task = [&](std::thread* thread) -> Task<int>
  {
    co_await AsyncExecuteAwaiter { thread };

    std::this_thread::sleep_for(std::chrono::milliseconds { 1 });

    count++;

    co_return 10;
  };

  auto make_task_void = [&](std::thread* thread) -> Task<>
  {
    co_await AsyncExecuteAwaiter { thread };

    std::this_thread::sleep_for(std::chrono::milliseconds { 1 });

    count++;
  };

  auto task = [&]() -> Task<int>
  {
    auto [value1, value2] = co_await WhenAll(make_task_void(&thread1), make_task(&thread2));

    static_assert(std::is_same_v<decltype(value1), VoidAwaitResult>, "Value should be void here");

    co_return value2;
  }();

  auto value = SyncWait(task);

  EXPECT_TRUE(task.IsReady());
  EXPECT_EQ(count, 2);
  EXPECT_EQ(value, 10);

  if (thread1.joinable()) thread1.join();
  if (thread2.joinable()) thread2.join();
}

TEST(WhenAllTask_Tests, WhenAll_DoubleRightVoid_CorrectValue)
{
  std::atomic_size_t count = 0;

  std::thread thread1;
  std::thread thread2;

  auto make_task = [&](std::thread* thread) -> Task<int>
  {
    co_await AsyncExecuteAwaiter { thread };

    std::this_thread::sleep_for(std::chrono::milliseconds { 1 });

    count++;

    co_return 10;
  };

  auto make_task_void = [&](std::thread* thread) -> Task<>
  {
    co_await AsyncExecuteAwaiter { thread };

    std::this_thread::sleep_for(std::chrono::milliseconds { 1 });

    count++;
  };

  auto task = [&]() -> Task<int>
  {
    auto [value1, value2] = co_await WhenAll(make_task(&thread1), make_task_void(&thread2));

    static_assert(std::is_same_v<decltype(value2), VoidAwaitResult>, "Value should be void here");

    co_return value1;
  }();

  auto value = SyncWait(task);

  EXPECT_TRUE(task.IsReady());
  EXPECT_EQ(count, 2);
  EXPECT_EQ(value, 10);

  if (thread1.joinable()) thread1.join();
  if (thread2.joinable()) thread2.join();
}

TEST(WhenAllTask_Tests, WhenAll_ManyAllVoid_Execute)
{
  std::atomic_size_t count = 0;

  std::thread thread[4];

  auto make_task = [&](std::thread* thread) -> Task<>
  {
    co_await AsyncExecuteAwaiter { thread };

    std::this_thread::sleep_for(std::chrono::milliseconds { 1 });

    count++;
  };

  auto task = [&]() -> Task<> {
    co_await WhenAll(make_task(&thread[0]), make_task(&thread[1]), make_task(&thread[2]), make_task(&thread[3]));
  }();

  SyncWait(task);

  EXPECT_TRUE(task.IsReady());
  EXPECT_EQ(count, 4);

  for (size_t i = 0; i < 4; i++)
  {
    if (thread[i].joinable()) thread[i].join();
  }
}

TEST(WhenAllTask_Tests, WhenAll_Many_CorrectValues)
{
  std::atomic_size_t count = 0;

  std::thread thread[4];

  auto make_task = [&](std::thread* thread) -> Task<int>
  {
    co_await AsyncExecuteAwaiter { thread };

    std::this_thread::sleep_for(std::chrono::milliseconds { 1 });

    count++;

    co_return 10;
  };

  auto task = [&]() -> Task<int>
  {
    auto [value1, value2, value3, value4] =
      co_await WhenAll(make_task(&thread[0]), make_task(&thread[1]), make_task(&thread[2]), make_task(&thread[3]));

    co_return value1 + value2 + value3 + value4;
  }();

  auto value = SyncWait(task);

  EXPECT_TRUE(task.IsReady());
  EXPECT_EQ(count, 4);
  EXPECT_EQ(value, 40);

  for (size_t i = 0; i < 4; i++)
  {
    if (thread[i].joinable()) thread[i].join();
  }
}

TEST(WhenAllTask_Tests, WhenAll_ManyOneVoid_CorrectValues)
{
  std::atomic_size_t count = 0;

  std::thread thread[4];

  auto make_task = [&](std::thread* thread) -> Task<int>
  {
    co_await AsyncExecuteAwaiter { thread };

    std::this_thread::sleep_for(std::chrono::milliseconds { 1 });

    count++;

    co_return 10;
  };

  auto make_task_void = [&](std::thread* thread) -> Task<>
  {
    co_await AsyncExecuteAwaiter { thread };

    std::this_thread::sleep_for(std::chrono::milliseconds { 1 });

    count++;
  };

  auto task = [&]() -> Task<int>
  {
    auto [value1, value2, value3, value4] =
      co_await WhenAll(make_task(&thread[0]), make_task_void(&thread[1]), make_task(&thread[2]), make_task(&thread[3]));

    static_assert(std::is_same_v<decltype(value2), VoidAwaitResult>, "Value should be void here");

    co_return value1 + value3 + value4;
  }();

  auto value = SyncWait(task);

  EXPECT_TRUE(task.IsReady());
  EXPECT_EQ(count, 4);
  EXPECT_EQ(value, 30);

  for (size_t i = 0; i < 4; i++)
  {
    if (thread[i].joinable()) thread[i].join();
  }
}

TEST(WhenAllTask_Tests, WhenAll_ManyAsRange_CorrectValues)
{
  std::atomic_size_t count = 0;

  static constexpr size_t amount = 100;

  std::thread thread[amount];

  auto make_task = [&](std::thread* thread) -> Task<int>
  {
    co_await AsyncExecuteAwaiter { thread };

    std::this_thread::sleep_for(std::chrono::milliseconds { 1 });

    count++;

    co_return 10;
  };

  auto task = [&]() -> Task<int>
  {
    FastVector<Task<int>> tasks;

    for (size_t i = 0; i < amount; i++)
    {
      tasks.PushBack(make_task(&thread[i]));
    }

    auto results = co_await WhenAll(std::move(tasks));

    int sum = 0;

    for (int result : results)
    {
      sum += result;
    }

    co_return sum;
  }();

  auto value = SyncWait(task);

  EXPECT_TRUE(task.IsReady());
  EXPECT_EQ(count, amount);
  EXPECT_EQ(value, 10 * amount);

  for (size_t i = 0; i < amount; i++)
  {
    if (thread[i].joinable()) thread[i].join();
  }
}

TEST(WhenAllTask_Tests, WhenAll_ManyVoidAsRange_CorrectValues)
{
  std::atomic_size_t count = 0;

  static constexpr size_t amount = 100;

  std::thread thread[amount];

  auto make_task = [&](std::thread* thread) -> Task<>
  {
    co_await AsyncExecuteAwaiter { thread };

    std::this_thread::sleep_for(std::chrono::milliseconds { 1 });

    count++;
  };

  auto task = [&]() -> Task<>
  {
    FastVector<Task<>> tasks;

    for (size_t i = 0; i < amount; i++)
    {
      tasks.PushBack(make_task(&thread[i]));
    }

    co_await WhenAll(std::move(tasks));
  }();

  SyncWait(task);

  EXPECT_TRUE(task.IsReady());
  EXPECT_EQ(count, amount);

  for (size_t i = 0; i < amount; i++)
  {
    if (thread[i].joinable()) thread[i].join();
  }
}

} // namespace genebits::engine