/* Minimal sys/socket.h compatibility for Windows builds */
#ifndef _SYS_SOCKET_H
#define _SYS_SOCKET_H
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#ifndef SHUT_WR
#define SHUT_WR SD_SEND
#endif
#ifndef SHUT_RD
#define SHUT_RD SD_RECEIVE
#endif
#ifndef SHUT_RDWR
#define SHUT_RDWR SD_BOTH
#endif

typedef int socklen_t;

#endif
#endif
