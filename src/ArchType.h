//
// Created by Haluk Akgunduz on 10/18/15.
//

#ifndef DICON_ARCHTYPE_H
#define DICON_ARCHTYPE_H

enum ARCH {

    ARCH_FREE,
    ARCH_LINUX_X86_64,
    ARCH_DARWIN_X86_64,
    ARCH_LINUX_ARM_32,
    ARCH_LINUX_ARM_64,
    ARCH_WINDOWS_X86_64,
    ARCH_MAX,
};

class ArchType {

    static constexpr const char *sArchTypes[ARCH_MAX] = {
            "arch-free",
            "x86-64-linux",
            "x86-64-darwin",
            "arm32-linux",
            "arm64-linux",
            "x86-64-windows",
    };

public:
    static const char *getName(ARCH _type) {

        return sArchTypes[_type];
    }

    static ARCH get() {

#if defined(__arm__) && defined(__ARM_32BIT_STATE) && defined(__linux__)
        return ARCH_LINUX_ARM_32;
#elif defined(__aarch64__) && defined(__linux__)
        return ARCH_LINUX_ARM_64;
#elif defined(__APPLE__)
        return ARCH_DARWIN_X86_64;
#elif defined(__x86_64__) && defined(__linux__)
        return ARCH_LINUX_X86_64;
#elif defined(WIN32)
        return ARCH_WINDOWS_X86_64;
#else
#error "Platform is not Supported!!!";
#endif
    }

};

#endif //DICON_ARCHTYPE_H
