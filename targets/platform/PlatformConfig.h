#pragma once

// Canonical platform flags used across the codebase.
// Keep this header lightweight and include-only so it can be used by both C
// and C++ translation units when needed.

#if defined(__ANDROID__)
#define APP_PLATFORM_ANDROID 1
#else
#define APP_PLATFORM_ANDROID 0
#endif

#if defined(__linux__) && !defined(__ANDROID__)
#define APP_PLATFORM_LINUX 1
#else
#define APP_PLATFORM_LINUX 0
#endif

#if defined(_WIN32) || defined(_WIN64)
#define APP_PLATFORM_WINDOWS 1
#else
#define APP_PLATFORM_WINDOWS 0
#endif
