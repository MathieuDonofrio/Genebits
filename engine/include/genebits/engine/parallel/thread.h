#ifndef GENEBITS_ENGINE_PARALLEL_THREAD_H
#define GENEBITS_ENGINE_PARALLEL_THREAD_H

#include <atomic>
#include <immintrin.h>
#include <thread>
#include <vector>

namespace genebits::engine
{
namespace this_thread
{
  ///
  /// Provides a hint to the processor that we are in a spin-wait loop.
  ///
  /// Can help improve performance and power consumption of spin-wait loops.
  ///
  inline void Pause() noexcept
  {
    _mm_pause();
  }

  ///
  /// Pauses for a given amount of cpu time in terms of loops.
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

  ///
  /// Sets the name of the thread for debugging purposes.
  ///
  /// @param[in] name Name to set.
  ///
  void SetName(const char* name);

} // namespace this_thread

///
/// Pointer to a native thread (platform-specific).
///
using NativeThreadHandle = void*;

///
/// Holds information about a physical processor.
///
struct ProcessorInfo
{
  uint64_t mask;
};

///
/// Holds information about a cache
///
struct CacheInfo
{
  uint32_t size;
  uint32_t count;
  uint32_t line_size;
};

///
/// Holds information about a physical processor.
///
struct CPUInfo
{
  std::vector<ProcessorInfo> processors;

  std::array<CacheInfo, 3> cache {};

  bool supported;
};

///
/// Obtains the native thread handle for platform specific threads.
///
/// @note If the platform is not supported, nullptr is returned.
///
/// @return Native thread handle.
///
NativeThreadHandle GetCurrentNativeThread();

///
/// Sets the thread affinity. Allows operating system to only use cores that are in the mask.
///
/// Every bit in the mask represents a processor.
///
/// @example 0b1 is processor 1 only.
///
/// @param[in] handle Native thread handle.
/// @param[in] mask Processor Mask.
///
/// @return True if the processor was set, false otherwise.
///
bool SetThreadAffinity(NativeThreadHandle handle, uint64_t mask);

///
/// Returns information about the CPU.
///
/// @return CPU information.
///
CPUInfo GetCPUInfo();

///
/// Tries to return the amount of physical processors. Not always accurate and will
/// sometimes return the amount of logical processors or a hint of amount of logical
/// processors.
///
/// @return Hint to amount of physical processors.
///
size_t GetAmountPhysicalProcessors();

///
/// Tries to return the amount of logical processors. Not always accurate and will return
/// a hint of amount of logical processors.
///
/// @return Hint to amount of logical processors.
///
size_t GetAmountLogicalProcessors();

} // namespace genebits::engine

#endif