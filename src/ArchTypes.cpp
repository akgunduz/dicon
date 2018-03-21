//
// Created by Haluk Akgunduz on 10/18/15.
//

#include "ArchTypes.h"

const char* sArchTypes[] = {
        "osx-x86",
        "win-x86",
        "lnx-x86",
        "lnx-arm11",
        "lnx-cortex",
};

const char *ArchTypes::getDir(ARCH arch) {
    return sArchTypes[arch];
}

short ArchTypes::getMax() {
    return ARCH_MAX;
}
