/* Minimal netinet/in.h compatibility for Windows builds */
#ifndef _NETINET_IN_H
#define _NETINET_IN_H
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#ifndef INADDR_NONE
#define INADDR_NONE 0xffffffff
#endif
#endif
#endif
