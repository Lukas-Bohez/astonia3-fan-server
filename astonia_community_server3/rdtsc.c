#if defined(_MSC_VER)
#include <intrin.h>
unsigned long long rdtsc(void) { return __rdtsc(); }
#elif defined(__GNUC__)
unsigned long long rdtsc(void) {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((unsigned long long)hi << 32) | lo;
}
#else
unsigned long long rdtsc(void) { return 0; }
#endif
