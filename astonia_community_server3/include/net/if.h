/* Minimal net/if.h compatibility for Windows builds */
#ifndef _NET_IF_H
#define _NET_IF_H
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>

/* Dummy definitions for interface flags */
#define IFNAMSIZ 16
struct ifreq { char ifr_name[IFNAMSIZ]; };
#endif
#endif
