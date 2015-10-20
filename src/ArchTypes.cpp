//
// Created by Haluk Akgunduz on 10/18/15.
//

#include "ArchTypes.h"

const char* sArchTypes[] = {
        "osx",
        "win",
        "lin",
        "arm11",
        "a7",
        "a8",
        "a9",
        "a15",
};

const char *ArchTypes::getDir(ARCH_IDS id) {
    return sArchTypes[id];
}

short ArchTypes::getMax() {
    return ARCH_MAX;
}
