//
// Created by Haluk Akgunduz on 10/18/15.
//

#ifndef BANKOR_ARCHTYPES_H
#define BANKOR_ARCHTYPES_H

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


#endif //BANKOR_ARCHTYPES_H
