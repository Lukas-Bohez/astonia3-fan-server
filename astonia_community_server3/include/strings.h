/* Minimal strings.h for Windows build */
#ifndef _STRINGS_H
#define _STRINGS_H

#include <string.h>

#ifdef _WIN32
#ifndef bzero
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
static inline void bzero(void *s, size_t n) { memset(s, 0, n); }
#pragma GCC diagnostic pop
#endif
#endif

#endif
