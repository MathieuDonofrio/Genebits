#ifndef GENEBITS_ENGINE_PARALLEL_TASK_H
#define GENEBITS_ENGINE_PARALLEL_TASK_H

#include "genebits/engine/parallel/thread.h"
#include "genebits/engine/util/delegate.h"

namespace genebits::engine
{
///
/// Delegate alias for executing tasks.
///
/// @see Delegate
///
using TaskExecutor = Delegate<>;

///
/// Task to be executed by thread pool.
///
/// @note Aligned on the cache line to avoid cache synchronization.
///
class alignas(std::hardware_destructive_interference_size) Task
{
public:
  ///
  /// Default constructor.
  ///
  constexpr Task() noexcept : next_(nullptr), flag_(false) {}

  ///
  /// Copy constructor.
  ///
  /// @param[in] other Task to copy.
  ///
  Task(const Task& other)
    : executor_(other.executor_), next_(other.next_), flag_(other.flag_.load(std::memory_order_relaxed))
  {}

  ///
  /// Copy assignment operator.
  ///
  /// @param[in] other Task to copy.
  ///
  /// @return Reference to this task.
  ///
  Task& operator=(const Task& other)
  {
    executor_ = other.executor_;
    next_ = other.next_;
    flag_ = other.flag_.load(std::memory_order_relaxed);

    return *this;
  }

  ///
  /// Spins for a fixed amount of specified spins.
  ///
  /// It may be an optimization to spin a little before waiting in certain cases such
  /// as small tasks.
  ///
  /// @param[in] spins Maximum amount of spins.
  ///
  /// @return True if the attempt to poll worked, false otherwise.
  ///
  bool TryPoll(size_t spins = 512) const noexcept
  {
    for (;;)
    {
      if (Finished()) return true;

      if (spins-- == 0) return false;

      this_thread::Pause();
    }
  }

  ///
  /// Spins until task is finished.
  ///
  /// @note Always prefer waiting to polling. Polling is considered dubious by many, however is
  /// can still be an optimization in certain cases.
  ///
  /// @warning High CPU usage when using this.
  ///
  void Poll() const noexcept
  {
    // Start with faster spinning
    for (size_t i = 0; i < 32; i++)
    {
      if (Finished()) return;

      this_thread::Pause();
    }

    // Fallback to yielding spins.
    while (!Finished())
    {
      std::this_thread::yield();
    }
  }

  ///
  /// Wait until the task is finished.
  ///
  /// If the task is not finished, this will block until notified.
  ///
  /// @note Does not use up CPU when waiting.
  ///
  void Wait() const noexcept
  {
    while (!Finished())
    {
      flag_.wait(false, std::memory_order_relaxed);
    }
  }

  ///
  /// Sets the state of the task to finished and notifies all waiting threads.
  ///
  void Finish()
  {
    ASSERT(!flag_.load(std::memory_order_relaxed), "Task already finished");

    flag_.store(true, std::memory_order_relaxed);
    flag_.notify_all();
  }

  ///
  /// Returns a reference to the task executor.
  ///
  /// @return Task executor reference.
  ///
  [[nodiscard]] TaskExecutor& Executor() noexcept
  {
    return executor_;
  }

  ///
  /// Returns whether or not this task is finished.
  ///
  /// @return True if task is finished, false otherwise.
  ///
  [[nodiscard]] bool Finished() const noexcept
  {
    return flag_.load(std::memory_order_relaxed);
  }

private:
  friend class TaskQueue;

  Delegate<> executor_;
  Task* next_;
  std::atomic_bool flag_;
};

///
/// Very low overhead task queue.
///
/// @note Not thread safe.
///
/// @note Does not perform any heap allocations.
///
class TaskQueue
{
public:
  ///
  /// Default constructor.
  ///
  constexpr TaskQueue() : front_(nullptr), back_(nullptr) {}

  TaskQueue(TaskQueue& other) = delete;
  TaskQueue& operator=(TaskQueue& other) = delete;

  ///
  /// Adds a task to the back of the queue.
  ///
  /// @param[in] task Task to add.
  ///
  constexpr void Push(Task* task) noexcept
  {
    ASSERT(!task->next_, "New task cannot have next task");

    if (front_) back_ = back_->next_ = task;
    else
    {
      front_ = back_ = task;
    }
  }

  ///
  /// Removes the task from the front of the queue.
  ///
  constexpr void Pop() noexcept
  {
    ASSERT(front_, "Queue cannot be empty");

    front_ = front_->next_;
  }

  ///
  /// Returns the task at the front of the queue.
  ///
  /// @return Task at the front of the queue, nullptr if the task is empty.
  ///
  [[nodiscard]] constexpr Task* Front() noexcept
  {
    return front_;
  }

  ///
  /// Returns whether or not the queue is empty.
  ///
  /// @return True if the queue is empty, false otherwise.
  ///
  [[nodiscard]] constexpr bool Empty() const noexcept
  {
    return !front_;
  }

private:
  Task* front_;
  Task* back_;
};
} // namespace genebits::engine

#endif