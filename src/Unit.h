//
// Created by Haluk Akgunduz on 10/17/15.
//

#ifndef BANKOR_UNIT_H
#define BANKOR_UNIT_H


#include "Common.h"

class Unit {
    int unit;

public:
    Unit();
    Unit(int unit);
    Unit(HOST type);
    Unit(HOST type, short id);
    Unit(const Unit& rep);
    HOST getType();
    short getID();
    int getUnit();
};


#endif //BANKOR_UNIT_H
