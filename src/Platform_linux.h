//
// Created by Haluk AKGUNDUZ on 10/5/2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_PLATFORM_LINUX_H
#define DICON_PLATFORM_LINUX_H

#include <climits>
#include <arpa/inet.h>
#include <sys/un.h>

#define ntohll(x) be64toh(x)
#define htonll(x) htobe64(x)

#define WEB_DOCUMENT_ROOT "../data/html/"
#define TEST_JOB_ZIP "../sample/Job1_x86_linux.zip"
#define TEST_JOB_PATH "../scratch/Job1_x86_linux"
#define TEST_JOB_FILE "matrop-multiply"

#define EXEC_EXT ""

#endif //DICON_PLATFORM_LINUX_H
