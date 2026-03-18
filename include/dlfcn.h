/* dlfcn compatibility for Windows (MinGW) */
#ifndef _DLFCN_COMPAT_H
#define _DLFCN_COMPAT_H
#ifdef _WIN32
#include <windows.h>
#include <limits.h>

#ifndef NAME_MAX
#define NAME_MAX 260
#endif

/* dlopen/dlsym wrapper using Win32 LoadLibrary/GetProcAddress */
typedef HMODULE dlhandle_t;

#ifndef RTLD_NOW
#define RTLD_NOW 0
#endif
#ifndef RTLD_LAZY
#define RTLD_LAZY 0
#endif

static inline dlhandle_t dlopen(const char *path, int flags) {
    (void)flags;
    return LoadLibraryA(path);
}
static inline void *dlsym(dlhandle_t handle, const char *symbol) {
    return (void*)GetProcAddress(handle, symbol);
}
static inline int dlclose(dlhandle_t handle) {
    return FreeLibrary(handle) ? 0 : -1;
}
static inline const char *dlerror(void) {
    return "dlerror: not supported on Windows";
}
#endif
#endif
