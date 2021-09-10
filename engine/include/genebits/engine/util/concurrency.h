#ifndef GENEBITS_ENGINE_UTIL_CONCURRENCY_H
#define GENEBITS_ENGINE_UTIL_CONCURRENCY_H

#include <atomic>
#include <immintrin.h>
#include <new>
#include <thread>

namespace genebits::engine
{
namespace this_thread
{
  ///
  /// Yields to other threads.
  ///
  /// Faster than std::this_thread::yield().
  ///
  inline void Pause() noexcept
  {
    _mm_pause();
  }

  ///
  /// Spins for the specified number of loops while yielding to other threads.
  ///
  /// @param[in] loops amount of loops to perform.
  ///
  inline void Pause(size_t loops) noexcept
  {
    while (loops-- != 0)
    {
      Pause();
    }
  }
} // namespace this_thread

///
/// Exponentially waits for an amount of yielding spins.
///
class ExponentialBackoff
{
public:
  static constexpr size_t cMaxWaitLoops = (1 << 5); // 32 (Power of two)

  ///
  /// Default Constructor
  ///
  constexpr ExponentialBackoff() : count_(0) {}

  ///
  /// Spins an amount of times while yielding. Amount of loops increase
  /// every time this method is called.
  ///
  /// @note Loop increase caps at some point.
  ///
  void Wait() noexcept
  {
    this_thread::Pause(count_ & (cMaxWaitLoops - 1));

    count_ <<= 1;
  }

  ///
  /// Returns whether or not the exponential backoff has reached the maximum wait time.
  ///
  /// @return True if reached max wait, false otherwise.
  ///
  [[nodiscard]] constexpr bool ReachedMaxWait() const noexcept
  {
    return count_ >= cMaxWaitLoops;
  }

private:
  size_t count_;
};

///
/// Very good spin lock implementation.
///
/// @note Implements the standard Lockable concept
///
class SpinMutex
{
public:
  ///
  /// Default constructor
  ///
  constexpr SpinMutex() : lock_ { false } {}

  ///
  /// Locks the spin lock, blocks if the lock is not available.
  ///
  void lock() noexcept
  {
    // Optimistically assume the lock is free on the first try
    if (try_lock()) return;

    ExponentialBackoff backoff;

    // Wait for lock to be released without generating cache misses
    while (lock_.load(std::memory_order_relaxed))
    {
      backoff.Wait();
    };
  }

  ///
  /// Tries to lock the spin lock. Returns immediately.
  ///
  /// @return True if aquired lock, false otherwise
  ///
  bool try_lock() noexcept
  {
    return !lock_.exchange(true, std::memory_order_acquire);
  }

  ///
  /// Unlocks the spin lock.
  ///
  void unlock() noexcept
  {
    lock_.store(false, std::memory_order_release);
  }

private:
  std::atomic<bool> lock_;
};

} // namespace genebits::engine

#endif
