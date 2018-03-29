//
// Created by Haluk Akgunduz on 10/17/15.
//

#ifndef BANKOR_UNIT_H
#define BANKOR_UNIT_H


#include "ComponentTypes.h"
#include "ArchTypes.h"

class Unit {
    COMPONENT type;
    ARCH arch;

    static char rootPath[COMP_MAX][PATH_MAX];

public:
    Unit();
    Unit(int);
    Unit(COMPONENT);
    Unit(COMPONENT, ARCH);
    Unit(const Unit&);
    COMPONENT getType();
    const char* getTypeName();
    ARCH getArch();
    int getUnit();

    static const char* getRootPath(COMPONENT);
    static void setRootPath(COMPONENT, const char*);
};


#endif //BANKOR_UNIT_H
