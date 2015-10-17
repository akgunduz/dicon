//
// Created by Haluk Akgunduz on 10/17/15.
//

#include "Unit.h"
#include "Util.h"

Unit::Unit(int unit) {
    this->unit = unit;
}

Unit::Unit(HOST type, short id) {
    this->unit = (int)type << 16 | id;
}

Unit::Unit(HOST type) {
    this->unit = (int)type << 16 | Util::getID();
}

HOST Unit::getType() {
    return (HOST) (unit >> 16);
}

short Unit::getID() {
    return (short) (unit & 0xFFFF);
}

Unit::Unit(const Unit &rep) {
    this->unit = rep.unit;
}

Unit::Unit() {

}

int Unit::getUnit() {
    return this->unit;
}
