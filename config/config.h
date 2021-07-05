#ifndef GENEBITS_CONFIG_CONFIG_HPP
#define GENEBITS_CONFIG_CONFIG_HPP

#include "version.h"

// Platform

#if defined(__linux__)
#define PLATFORM_LINUX 1
#define PLATFORM_WINDOWS 0
#define PLATFORM_APPLE 0
#elif defined(_WIN32)
#define PLATFORM_LINUX 0
#define PLATFORM_WINDOWS 1
#define PLATFORM_APPLE 0
#elif defined(__APPLE__)
#define PLATFORM_WINDOWS 0
#define PLATFORM_LINUX 0
#define PLATFORM_APPLE 1
#else
#error Platform not supported
#endif

// Compilers

#if defined(__clang__)
#define COMPILER_CLANG 1
#define COMPILER_GCC 0
#define COMPILER_MSVC 0
#elif defined(__GNUC__) || defined(__GNUG__)
#define COMPILER_CLANG 0
#define COMPILER_GCC 1
#define COMPILER_MSVC 0
#elif defined(_MSC_VER)
#define COMPILER_CLANG 0
#define COMPILER_GCC 0
#define COMPILER_MSVC 1
#else
#error Compiler not supported
#endif

// C++ Standard

#if defined(__cplusplus)
#if COMPILER_MSVC
#if __cplusplus > _MSVC_LANG
#define CPP_STANDARD __cplusplus // __STDC_VERSION__ ?
#else
#define CPP_STANDARD _MSVC_LANG
#endif
#else
#define CPP_STANDARD __cplusplus
#endif
#if CPP_STANDARD > 201703L
#define CPP_20 1
#else
#error Pre C++20 not supported
#endif
#else
#error Non standard C++ not supported
#endif

// Inline

#if COMPILER_GCC || COMPILER_CLANG
#define FORCEINLINE inline __attribute__((always_inline))
#else
#define FORCEINLINE inline
#endif

// SIMD

#if defined(__AVX512F__)
#define SIMD_WIDTH 16
#elif defined(__AVX2__) || defined(__AVX__)
#define SIMD_WIDTH 8
#elif defined(__SSE4_2__)
#define SIMD_WIDTH 4
#else
#define SIMD_WIDTH 1
#endif

// Assert

#ifdef BUILD_DEBUG
#include <cassert>
#define ASSERT(condition, ...) assert(condition)
#else
#define ASSERT(...) (void(0))
#endif

#endif