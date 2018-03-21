//
// Created by Haluk Akgunduz on 10/18/15.
//

#ifndef BANKOR_ARCHTYPES_H
#define BANKOR_ARCHTYPES_H

enum ARCH {
    ARCH_OSX_X86,
    ARCH_WIN_X86,
    ARCH_LNX_X86,
    ARCH_LNX_ARM11,
    ARCH_LNX_CORTEX,
    ARCH_MAX,
};

class ArchTypes {

public:
    static const char* getDir(ARCH id);
    static short getMax();
};


#endif //BANKOR_ARCHTYPES_H
