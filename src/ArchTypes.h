//
// Created by Haluk Akgunduz on 10/18/15.
//

#ifndef BANKOR_ARCHTYPES_H
#define BANKOR_ARCHTYPES_H

enum ARCH_IDS {
    ARCH_OSX,
    ARCH_WIN,
    ARCH_LIN,
    ARCH_ARM11,
    ARCH_A7,
    ARCH_A8,
    ARCH_A9,
    ARCH_A15,
    ARCH_MAX
};

class ArchTypes {

public:
    static const char* getDir(ARCH_IDS id);
    static short getMax();
};


#endif //BANKOR_ARCHTYPES_H
