//
// Created by Haluk Akgunduz on 10/18/15.
//

#include "ArchTypes.h"

const char* sArchTypes[] = {
        "osx-x86",
        "win-x86",
        "lnx-x86",
        "lnx-arm11",
        "lnx-a7",
        "lnx-a8",
        "lnx-a9",
        "lnx-a15",
};

const char *ArchTypes::getDir(ARCH_IDS id) {
    return sArchTypes[id];
}

short ArchTypes::getMax() {
    return ARCH_MAX;
}
