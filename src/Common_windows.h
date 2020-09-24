//
// Created by akgun on 24.08.2020.
//

#ifndef DICON_COMMONWINDOWS_H
#define DICON_COMMONWINDOWS_H

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#ifndef MAX_INPUT
#define MAX_INPUT 256
#endif

#include <Winsock2.h>
#include <Ws2ipdef.h>
#include <Ws2tcpip.h>
#include <Iphlpapi.h>
#include <afunix.h>
#include <io.h>

#define WEB_DOCUMENT_ROOT "../../../data/html/"
#define TEST_JOB_ZIP "../../../sample/Job1_x86_windows.zip"
#define TEST_JOB_PATH "../../../scratch/Job1_x86_windows"
#define TEST_JOB_FILE "matrop-multiply"

#endif //DICON_COMMONWINDOWS_H
