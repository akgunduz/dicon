//
// Created by Haluk AKGUNDUZ on 10/5/2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_PLATFORM_WINDOWS_H
#define DICON_PLATFORM_WINDOWS_H

#include <Winsock2.h>
#include <Ws2ipdef.h>
#include <Ws2tcpip.h>
#include <afunix.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#ifndef NAME_MAX
#define NAME_MAX 255
#endif

#ifndef MAX_INPUT
#define MAX_INPUT 255
#endif

#define WEB_DOCUMENT_ROOT "../data/html/"
#define TEST_JOB_ZIP "../sample/Job1_x86_windows.zip"
#define TEST_JOB_PATH "../scratch/Job1_x86_windows"
#define TEST_JOB_FILE "matrop-multiply"

#endif //DICON_PLATFORM_WINDOWS_H
