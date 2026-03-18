/* Minimal signal.h for Windows build */
#ifndef _SIGNAL_H
#define _SIGNAL_H

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>

typedef int sig_atomic_t;
typedef int sigset_t;

#define SIGINT 2
#define SIGTERM 15
#define SIGPIPE 13
#define SIGSEGV 11
#define SIGFPE 8
#define SIGBUS 7
#ifndef SIGSTKFLT
#define SIGSTKFLT 16
#endif

#define SIG_IGN ((void (*)(int))1)

static inline void (*signal(int sig, void (*func)(int)))(int) {
    (void)sig; (void)func;
    return SIG_IGN;
}

static inline int sigemptyset(sigset_t *set) {
    if (!set) return -1;
    *set = 0;
    return 0;
}

static inline int sigfillset(sigset_t *set) {
    if (!set) return -1;
    *set = ~0;
    return 0;
}

static inline int sigdelset(sigset_t *set, int signo) {
    if (!set) return -1;
    *set &= ~(1 << signo);
    return 0;
}

static inline int pthread_sigmask(int how, const sigset_t *set, sigset_t *oldset) {
    (void)how; (void)set; (void)oldset;
    return 0;
}

#endif

#endif
