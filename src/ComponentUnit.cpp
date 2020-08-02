//
// Created by akgun on 7.07.2020.
//

#include "ComponentUnit.h"

ComponentUnit::ComponentUnit(COMPONENT _type, long _id, Address _address)
    : id(_id), type(_type), address(_address) {
}

ComponentUnit::ComponentUnit(COMPONENT _type)
    : type(_type) {
}

ComponentUnit::ComponentUnit(const ComponentUnit &copy) = default;

long ComponentUnit::getID() const {

    return id;
}

void ComponentUnit::setID(long _id) {

    id = _id;
}

COMPONENT ComponentUnit::getType() const {

    return (COMPONENT)type;
}

void ComponentUnit::setType(COMPONENT _type) {

    type = _type;
}

Address& ComponentUnit::getAddress() {

    return address;
}

void ComponentUnit::setAddress(Address _address, bool isMulticast) {

    address = _address;
    if (isMulticast) {
        address.setMulticast(true);
    }
}

COMPONENT ComponentUnit::next(COMPONENT component) {

    return COMPONENT (((int)component + 1) % COMP_MAX);
}

COMPONENT ComponentUnit::prev(COMPONENT component) {

    return COMPONENT (((int)component + COMP_MAX - 1) % COMP_MAX);
}

void ComponentUnit::grab(ComponentUnit &unit) {

    id = unit.id;
    type = unit.type;
    address.set(unit.address.get());
    address.setUI(unit.address.getUI());
}
