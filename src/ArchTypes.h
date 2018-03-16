//
// Created by Haluk Akgunduz on 10/18/15.
//

#ifndef BANKOR_ARCHTYPES_H
#define BANKOR_ARCHTYPES_H

enum ARCH {
    ARCH_OSX,
    ARCH_WIN,
    ARCH_LNX,
    ARCH_A11,
    ARCH_CA7,
    ARCH_CA8,
    ARCH_CA9,
    ARCH_CA15,
    ARCH_MAX,
};

class ArchTypes {

public:
    static const char* getDir(ARCH id);
    static short getMax();
};


#endif //BANKOR_ARCHTYPES_H
