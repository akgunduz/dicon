//
// Created by Haluk AKGUNDUZ on 3.08.2020.
//

#include "CommUnit.h"

CommUnit::CommUnit(COMPONENT _type, ARCH _arch, TypeID _id, Address _address)
        : id(_id), type(_type), arch(_arch), address(_address) {
}

CommUnit::CommUnit(COMPONENT _type, ARCH _arch, TypeID _id)
        : id(_id), type(_type), arch(_arch) {
}

CommUnit::CommUnit(COMPONENT _type, Address _address)
        : type(_type), address(_address) {
}

CommUnit::CommUnit(COMPONENT _type)
        : type(_type) {
}

TypeID CommUnit::getID() {

    return id;
}

void CommUnit::setID(TypeID _id) {

    id = _id;
}

ARCH CommUnit::getArch() {

    return (ARCH) arch;
}

void CommUnit::setArch(ARCH _arch) {

    arch = _arch;
}

COMPONENT CommUnit::getType() {

    return (COMPONENT)type;
}

void CommUnit::setType(COMPONENT _type) {

    type = _type;
}

Address& CommUnit::getAddress() {

    return address;
}

void CommUnit::setAddress(Address _address, bool isMulticast) {

    address = _address;
    address.setMulticast(isMulticast);
}

COMPONENT CommUnit::next(COMPONENT component) {

    return COMPONENT (((int)component + 1) % COMP_MAX);
}

COMPONENT CommUnit::prev(COMPONENT component) {

    return COMPONENT (((int)component + COMP_MAX - 1) % COMP_MAX);
}

void CommUnit::set(const CommUnit &unit) {

    *this = unit;
}
