/* Minimal arpa/inet.h for Windows builds */
#ifndef _ARPA_INET_H
#define _ARPA_INET_H

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#endif
