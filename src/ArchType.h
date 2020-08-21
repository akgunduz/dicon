//
// Created by Haluk Akgunduz on 10/18/15.
//

#ifndef DICON_ARCHTYPE_H
#define DICON_ARCHTYPE_H

enum ARCH {
    ARCH_LINUX_X86_64,
    ARCH_DARWIN_X86_64,
    ARCH_LINUX_ARM_32,
    ARCH_LINUX_ARM_64,
    ARCH_MAX,
};

class ArchType {

public:
    static const char* getName(ARCH id);
    static ARCH get();
};

#endif //DICON_ARCHTYPE_H
