/* Minimal ioctl compatibility for Windows builds */
#ifndef _SYS_IOCTL_H
#define _SYS_IOCTL_H
#ifdef _WIN32
#include <winsock2.h>
/* Provide a no-op ioctl replacement for code that expects it on Unix. */
static inline int ioctl(int fd, unsigned long request, void *arg) {
    (void)fd; (void)request; (void)arg;
    return 0;
}
#endif
#endif
