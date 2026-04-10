#pragma once

#include <cstdarg>
#include <cstdio>

namespace Log {

inline void info(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    std::vfprintf(stderr, fmt, args);
    va_end(args);
}

}  // namespace Log
