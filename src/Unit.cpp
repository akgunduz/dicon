//
// Created by Haluk Akgunduz on 10/17/15.
//

#include "Unit.h"
#include "Util.h"

char Unit::rootPath[COMP_MAX][PATH_MAX];

Unit::Unit(int data) {
    this->type = (COMPONENT) (data >> 16);
    this->arch = (ARCH) (data & 0xFFFF);
}

Unit::Unit(COMPONENT component, ARCH arch) {

    this->type = component;
    this->arch = arch;

    //this->unit = (int)type << 16 | arch;
}

Unit::Unit(COMPONENT component) {

    this->type = component;
    this->arch = Util::getArch();
   // this->unit = (int)type << 16 | Util::getArch();
}

COMPONENT Unit::getType() {

    return type;
   // return (COMPONENT) (unit >> 16);
}

ARCH Unit::getArch() {

    return arch;
 //   return (short) (unit & 0xFFFF);
}

Unit::Unit(const Unit &rep) {
    this->type = rep.type;
    this->arch = rep.arch;
    //this->unit = rep.unit;
}

Unit::Unit() {

}

int Unit::getUnit() {
    return type << 16 | arch;
}

const char* Unit::getRootPath(COMPONENT component) {
    return rootPath[component];
}

void Unit::setRootPath(COMPONENT component, const char *path) {
    strcpy(rootPath[component], path);
}


