/* Minimal sys/time.h for Windows build */
#ifndef _SYS_TIME_H
#define _SYS_TIME_H
#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>

struct timeval {
    long tv_sec;
    long tv_usec;
};

static __attribute__((unused)) int gettimeofday(struct timeval *tp, void *tzp) {
    if (!tp) return -1;
    FILETIME ft;
    unsigned long long tmp;
    GetSystemTimeAsFileTime(&ft);
    tmp = ((unsigned long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    /* FILETIME is 100-nanosecond intervals since Jan 1, 1601 */
    tmp -= 116444736000000000ULL; /* convert to Unix epoch */
    tp->tv_sec = (long)(tmp / 10000000ULL);
    tp->tv_usec = (long)((tmp % 10000000ULL) / 10);
    return 0;
}

/* select is provided by Winsock */
#endif
#endif
