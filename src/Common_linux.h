//
// Created by Haluk AKGUNDUZ on 09/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_COMMONLINUX_H
#define DICON_COMMONLINUX_H

#define ntohll(x) be64toh(x)
#define htonll(x) htobe64(x)

#ifndef MD5_DIGEST_LENGTH
#define MD5_DIGEST_LENGTH 16
#endif

#define UNIXSOCKET_PATH "/var/tmp/"
#define UNIXSOCKET_FILE_PREFIX "dicon_"
#define UNIXSOCKET_FILE_SUFFIX ".sock"

#define DISTRIBUTOR_PATH "Distributor"
#define COLLECTOR_PATH "Collector"
#define NODE_PATH "Node"

#endif //DICON_COMMONLINUX_H
