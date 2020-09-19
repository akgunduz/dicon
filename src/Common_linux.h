//
// Created by Haluk AKGUNDUZ on 09/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_COMMONLINUX_H
#define DICON_COMMONLINUX_H

#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

#include "pstreams/pstream.h"

#define ntohll(x) be64toh(x)
#define htonll(x) htobe64(x)

#endif //DICON_COMMONLINUX_H
