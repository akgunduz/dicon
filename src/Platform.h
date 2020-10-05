//
// Created by Haluk AKGUNDUZ on 10/5/2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_PLATFORM_H
#define DICON_PLATFORM_H

#if defined(__APPLE__)
#include "Platform_macos.h"
#elif defined(WIN32)
#include "Platform_windows.h"
#else
#include "Platform_linux.h"
#endif

#endif //DICON_PLATFORM_H
