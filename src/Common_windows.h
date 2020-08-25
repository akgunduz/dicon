//
// Created by akgun on 24.08.2020.
//

#ifndef DICON_COMMONWINDOWS_H
#define DICON_COMMONWINDOWS_H

#define _CRT_SECURE_NO_WARNINGS

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define NOMINMAX

#include <Winsock2.h>
#include <Ws2ipdef.h>
#include <Ws2tcpip.h>
#include <Iphlpapi.h>
#include <afunix.h>
#include <io.h>

#define pipe(a) _pipe(a, 256, O_BINARY)
#define open _open
#define read _read
#define write _write
#define close _close
#define socklen_t int

#define SHUT_RDWR SD_BOTH

#endif //DICON_COMMONWINDOWS_H
