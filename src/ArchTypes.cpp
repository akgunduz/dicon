//
// Created by Haluk Akgunduz on 10/18/15.
//

#include "ArchTypes.h"

const char* sArchTypes[] = {
        "x86-64-linux",
        "x86-64-darwin",
        "arm32-linux",
        "arm64-linux",
};

const char *ArchTypes::getName(ARCH arch) {
    return sArchTypes[arch];
}

ARCH ArchTypes::get() {

#if defined(__arm__) && defined(__ARM_32BIT_STATE) && defined(__linux__)
    return ARCH_LINUX_ARM_32;
#elif defined(__aarch64__) && defined(__linux__)
    return ARCH_LINUX_ARM_64;
#elif defined(__APPLE__)
    return ARCH_DARWIN_X86_64;
#elif defined(__x86_64__) && defined(__linux__)
    return ARCH_LINUX_X86_64;
#else
    #error "Platform is not Supported!!!";
#endif
}
