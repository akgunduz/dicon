//
// Created by Haluk AKGUNDUZ on 17.09.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#include "BaseUnit.h"

BaseUnit::BaseUnit(COMPONENT _type, ARCH _arch, TypeID _id)
        : id(_id), type(_type), arch(_arch) {
}

BaseUnit::BaseUnit(COMPONENT _type)
        : type(_type) {
}

BaseUnit::~BaseUnit() {}

TypeID BaseUnit::getID() {

    return id;
}

void BaseUnit::setID(TypeID _id) {

    id = _id;
}

ARCH BaseUnit::getArch() {

    return (ARCH) arch;
}

void BaseUnit::setArch(ARCH _arch) {

    arch = _arch;
}

COMPONENT BaseUnit::getType() {

    return (COMPONENT)type;
}

void BaseUnit::setType(COMPONENT _type) {

    type = _type;
}

COMPONENT BaseUnit::next(COMPONENT component) {

    return COMPONENT (((int)component + 1) % COMP_MAX);
}

COMPONENT BaseUnit::prev(COMPONENT component) {

    return COMPONENT (((int)component + COMP_MAX - 1) % COMP_MAX);
}