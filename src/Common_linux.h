//
// Created by Haluk AKGUNDUZ on 09/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef __Common_LINUX_H_
#define __Common_LINUX_H_

#include <openssl/md5.h>
#define ntohll(x) be64toh(x)
#define htonll(x) htobe64(x)

#define UNIXSOCKET_PATH "/var/tmp/"
#define UNIXSOCKET_FILE_PREFIX "bankor_"
#define UNIXSOCKET_FILE_SUFFIX ".sock"

#define DISTRIBUTOR_PATH "Distributor"
#define COLLECTOR_PATH "Collector"
#define NODE_PATH "Node"
#endif
