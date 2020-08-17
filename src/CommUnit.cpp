//
// Created by Haluk AKGUNDUZ on 3.08.2020.
//

#include "CommUnit.h"

CommUnit::CommUnit(COMPONENT _type, ARCH _arch, long _id, Address _address)
        : id(_id), type(_type), arch(_arch), address(_address) {
}

CommUnit::CommUnit(COMPONENT _type)
        : type(_type) {
}

CommUnit::CommUnit(const CommUnit &copy) = default;

long CommUnit::getID() const {

    return id;
}

void CommUnit::setID(long _id) {

    id = _id;
}

ARCH CommUnit::getArch() const {

    return (ARCH) arch;
}

void CommUnit::setArch(ARCH _arch) {

    arch = _arch;
}

COMPONENT CommUnit::getType() const {

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
    if (isMulticast) {
        address.setMulticast(true);
    }
}

COMPONENT CommUnit::next(COMPONENT component) {

    return COMPONENT (((int)component + 1) % COMP_MAX);
}

COMPONENT CommUnit::prev(COMPONENT component) {

    return COMPONENT (((int)component + COMP_MAX - 1) % COMP_MAX);
}

void CommUnit::grab(CommUnit &unit) {

    id = unit.id;
    arch = unit.arch;
    type = unit.type;
    address.set(unit.address.get());
    address.setUI(unit.address.getUI());
}
