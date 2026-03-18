/* Minimal sys/wait.h for Windows builds */
#ifndef _SYS_WAIT_H
#define _SYS_WAIT_H

#ifdef _WIN32
#include <sys/types.h>

#define WNOHANG 1
#define WUNTRACED 2

#define WEXITSTATUS(status) ((status) & 0xff)
#define WIFEXITED(status) (((status) & 0xff) == 0)

static inline pid_t waitpid(pid_t pid, int *status, int options) {
    (void)pid; (void)options;
    if (status) *status = 0;
    return -1;
}

#endif

#endif
