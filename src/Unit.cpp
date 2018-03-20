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
}

Unit::Unit(COMPONENT component) {

    this->type = component;
    this->arch = Util::getArch();
}

COMPONENT Unit::getType() {

    return type;
}

ARCH Unit::getArch() {

    return arch;
}

Unit::Unit(const Unit &rep) {
    this->type = rep.type;
    this->arch = rep.arch;
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


