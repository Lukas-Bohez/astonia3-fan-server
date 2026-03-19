/* Minimal unistd.h for Windows build */
#ifndef _UNISTD_H
#define _UNISTD_H

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <process.h>

/* Use MinGW-provided getpid/kill if available */
#ifdef _MSC_VER
static inline int getpid(void) { return _getpid(); }
#endif

#ifndef kill
static inline int kill(int pid, int sig) {
    (void)sig;
    return TerminateProcess((HANDLE)pid, 1) ? 0 : -1;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
static inline int fork(void) { return -1; }
#pragma GCC diagnostic pop
#endif

static inline unsigned int sleep(unsigned int seconds) {
    Sleep(seconds * 1000);
    return 0;
}

static inline int close(int fd) {
    return closesocket(fd);
}

#endif

#endif
