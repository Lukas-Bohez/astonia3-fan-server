/* Minimal netdb.h compatibility for Windows builds */
#ifndef _NETDB_H
#define _NETDB_H
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>

struct hostent;
struct addrinfo;

#endif
#endif
