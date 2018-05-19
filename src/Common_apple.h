//
// Created by Haluk AKGUNDUZ on 09/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//


#ifndef BANKOR_COMMONAPPLE_H
#define BANKOR_COMMONAPPLE_H

#include <IOKit/IOKitKeys.h>
#include <CommonCrypto/CommonDigest.h>

#ifdef MD5_DIGEST_LENGTH
#undef MD5_DIGEST_LENGTH
#endif

#define MD5_Init            CC_MD5_Init
#define MD5_Update          CC_MD5_Update
#define MD5_Final           CC_MD5_Final
#define MD5_DIGEST_LENGTH   CC_MD5_DIGEST_LENGTH
#define MD5_CTX             CC_MD5_CTX

#define UNIXSOCKET_PATH "/var/tmp/"
#define UNIXSOCKET_FILE_PREFIX "bankor_"
#define UNIXSOCKET_FILE_SUFFIX ".sock"

#define DISTRIBUTOR_PATH "Distributor"
#define COLLECTOR_PATH "Collector"
#define NODE_PATH "Node"

#endif
