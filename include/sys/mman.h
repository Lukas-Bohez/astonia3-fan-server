/* Minimal sys/mman.h compatibility for Windows builds */
#ifndef _SYS_MMAN_H
#define _SYS_MMAN_H
#ifdef _WIN32
#include <windows.h>

#define PROT_READ  0x1
#define PROT_WRITE 0x2

#define MAP_SHARED  0x01
#define MAP_PRIVATE 0x02
#define MAP_ANONYMOUS 0x20
#define MAP_ANON MAP_ANONYMOUS

#define MAP_FAILED ((void*)-1)

static inline void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    (void)addr; (void)fd; (void)offset; (void)flags;
    DWORD protect = 0;
    if (prot & PROT_READ) protect = PAGE_READONLY;
    if (prot & PROT_WRITE) protect = PAGE_READWRITE;
    if (protect == 0) protect = PAGE_READONLY;
    void *p = VirtualAlloc(NULL, length, MEM_COMMIT | MEM_RESERVE, protect);
    return p ? p : MAP_FAILED;
}

static inline int mprotect(void *addr, size_t len, int prot) {
    (void)addr; (void)len; (void)prot;
    return 0;
}

static inline int munmap(void *addr, size_t length) {
    (void)length;
    return VirtualFree(addr, 0, MEM_RELEASE) ? 0 : -1;
}

#endif
#endif
