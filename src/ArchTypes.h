//
// Created by Haluk Akgunduz on 10/18/15.
//

#ifndef DICON_ARCHTYPES_H
#define DICON_ARCHTYPES_H

enum ARCH {
    ARCH_MACOS_X86_64,
    ARCH_WINDOWS_X86_64,
    ARCH_LINUX_X86_64,
    ARCH_LINUX_ARM_32,
    ARCH_MAX,
};

class ArchTypes {

public:
    static const char* getDir(ARCH id);
    static short getMax();
    static ARCH getNative();
};


#endif //DICON_ARCHTYPES_H
