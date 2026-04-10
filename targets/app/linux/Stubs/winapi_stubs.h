#ifndef WINAPISTUBS_H
#define WINAPISTUBS_H

#pragma once

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include <cstdint>
#include <ctime>

typedef struct {
    int32_t LowPart;
    int32_t HighPart;
    int64_t QuadPart;
} LARGE_INTEGER;

typedef struct {
    uint32_t LowPart;
    uint32_t HighPart;
    uint64_t QuadPart;
} ULARGE_INTEGER;

#define ERROR_SUCCESS 0L
#define ERROR_IO_PENDING 997L  // dderror
#define ERROR_CANCELLED 1223L

#define MEM_COMMIT 0x1000
#define MEM_RESERVE 0x2000
#define MEM_DECOMMIT 0x4000
#define PAGE_READWRITE 0x04

// https://learn.microsoft.com/en-us/windows/win32/api/minwinbase/ns-minwinbase-filetime
typedef struct _FILETIME {
    int32_t dwLowDateTime;
    int32_t dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;

// https://learn.microsoft.com/en-us/windows/win32/api/minwinbase/ns-minwinbase-systemtime
typedef struct _SYSTEMTIME {
    int16_t wYear;
    int16_t wMonth;
    int16_t wDayOfWeek;
    int16_t wDay;
    int16_t wHour;
    int16_t wMinute;
    int16_t wSecond;
    int16_t wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

#ifdef __LP64__
static inline int64_t InterlockedCompareExchangeRelease64(
    int64_t volatile* Destination, int64_t Exchange, int64_t Comperand) {
    int64_t expected = Comperand;
    __atomic_compare_exchange_n(Destination, &expected, Exchange, false,
                                __ATOMIC_RELEASE, __ATOMIC_RELAXED);
    return expected;
}
#else
static inline int64_t InterlockedCompareExchangeRelease(
    LONG volatile* Destination, LONG Exchange, LONG Comperand) {
    LONG expected = Comperand;
    __atomic_compare_exchange_n(Destination, &expected, Exchange, false,
                                __ATOMIC_RELEASE, __ATOMIC_RELAXED);
    return expected;
}
#endif

// internal helper: convert time_t to FILETIME (100ns intervals since
// 1601-01-01)
static inline FILETIME _TimeToFileTime(time_t t) {
    const uint64_t EPOCH_DIFF = 11644473600ULL;
    uint64_t val = ((uint64_t)t + EPOCH_DIFF) * 10000000ULL;
    FILETIME ft;
    ft.dwLowDateTime = (int32_t)(val & 0xFFFFFFFF);
    ft.dwHighDateTime = (int32_t)(val >> 32);
    return ft;
}

// internal helper: convert FILETIME (100ns since 1601) to time_t (seconds since
// 1970)
static inline time_t _FileTimeToTimeT(const FILETIME& ft) {
    uint64_t val = ((uint64_t)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    const uint64_t EPOCH_DIFF =
        116444736000000000ULL;  // 100ns intervals between 1601-01-01 and
                                // 1970-01-01
    return (time_t)((val - EPOCH_DIFF) / 10000000ULL);
}

// internal helper: read the current wall clock into a timespec
static inline void _CurrentTimeSpec(struct timespec* ts) {
#ifdef CLOCK_REALTIME
    clock_gettime(CLOCK_REALTIME, ts);
#else
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    ts->tv_sec = tv.tv_sec;
    ts->tv_nsec = tv.tv_usec * 1000;
#endif
}

// internal helper: fill SYSTEMTIME from a broken-down tm + nanosecond remainder
static inline void _FillSystemTime(const struct tm* tm, long tv_nsec,
                                   LPSYSTEMTIME lpSystemTime) {
    lpSystemTime->wYear = tm->tm_year + 1900;
    lpSystemTime->wMonth = tm->tm_mon + 1;
    lpSystemTime->wDayOfWeek = tm->tm_wday;  // 0 = Sunday
    lpSystemTime->wDay = tm->tm_mday;
    lpSystemTime->wHour = tm->tm_hour;
    lpSystemTime->wMinute = tm->tm_min;
    lpSystemTime->wSecond = tm->tm_sec;
    lpSystemTime->wMilliseconds = (int16_t)(tv_nsec / 1000000);  // ns to ms
}

// https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getlocaltime
static inline void GetLocalTime(LPSYSTEMTIME lpSystemTime) {
    struct timespec ts;
    _CurrentTimeSpec(&ts);
    struct tm tm;
    localtime_r(&ts.tv_sec, &tm);  // local time
    _FillSystemTime(&tm, ts.tv_nsec, lpSystemTime);
}

// https://learn.microsoft.com/en-us/windows/win32/api/timezoneapi/nf-timezoneapi-filetimetosystemtime
static inline bool FileTimeToSystemTime(const FILETIME* lpFileTime,
                                        LPSYSTEMTIME lpSystemTime) {
    uint64_t ft = ((uint64_t)lpFileTime->dwHighDateTime << 32) |
                  lpFileTime->dwLowDateTime;
    time_t t = _FileTimeToTimeT(*lpFileTime);
    long remainder_ns = (long)((ft % 10000000ULL) * 100);

    struct tm tm;
    gmtime_r(&t, &tm);  // UTC
    _FillSystemTime(&tm, remainder_ns, lpSystemTime);
    return true;
}

static inline void* VirtualAlloc(void* lpAddress, size_t dwSize,
                                 int32_t flAllocationType, int32_t flProtect) {
    // MEM_COMMIT | MEM_RESERVE → mmap anonymous
    int prot = 0;
    if (flProtect == 0x04 /*PAGE_READWRITE*/)
        prot = PROT_READ | PROT_WRITE;
    else if (flProtect == 0x40 /*PAGE_EXECUTE_READWRITE*/)
        prot = PROT_READ | PROT_WRITE | PROT_EXEC;
    else if (flProtect == 0x02 /*PAGE_READONLY*/)
        prot = PROT_READ;
    else
        prot = PROT_READ | PROT_WRITE;  // default

    int flags = MAP_PRIVATE | MAP_ANONYMOUS;
    if (lpAddress != nullptr) flags |= MAP_FIXED;

    void* p = mmap(lpAddress, dwSize, prot, flags, -1, 0);
    if (p == MAP_FAILED) return nullptr;
    return p;
}

static inline bool VirtualFree(void* lpAddress, size_t dwSize,
                               int32_t dwFreeType) {
    if (lpAddress == nullptr) return false;
    // MEM_RELEASE (0x8000) frees the whole region
    if (dwFreeType == 0x8000 /*MEM_RELEASE*/) {
        // dwSize should be 0 for MEM_RELEASE per Win32 API, but we don't track
        // allocation sizes Use dwSize if provided, otherwise this is a
        // best-effort
        if (dwSize == 0) dwSize = 4096;  // minimum page
        munmap(lpAddress, dwSize);
    } else {
        // MEM_DECOMMIT (0x4000) - just decommit (make inaccessible)
        madvise(lpAddress, dwSize, MADV_DONTNEED);
    }
    return true;
}

#endif  // WINAPISTUBS_H
