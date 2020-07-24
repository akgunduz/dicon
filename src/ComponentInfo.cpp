//
// Created by akgun on 7.07.2020.
//

#include "ComponentInfo.h"

ComponentInfo::ComponentInfo(COMPONENT _type, long _id, long _address) :
    id(_id), type(_type) {

    address[_type] = _address;
}

ComponentInfo::ComponentInfo(const ComponentInfo &copy) {

    type = copy.getType();
    id = copy.getID();
    address[COMP_DISTRIBUTOR] = copy.getAddress(COMP_DISTRIBUTOR);
    address[COMP_COLLECTOR] = copy.getAddress(COMP_COLLECTOR);
    address[COMP_NODE] = copy.getAddress(COMP_NODE);
}

long ComponentInfo::getID() const {

    return id;
}

void ComponentInfo::setID(long _id) {

    id = _id;
}

COMPONENT ComponentInfo::getType() const {

    return type;
}

void ComponentInfo::setType(COMPONENT _type) {

    type = _type;
}

long ComponentInfo::getAddress(COMPONENT component) const {

    return address[component];
}

void ComponentInfo::setAddress(COMPONENT component, long _address) {

    address[component] = _address;
}

long ComponentInfo::getAddress() const {

    return address[type];
}

void ComponentInfo::setAddress(long _address) {

    address[type] = _address;
}

COMPONENT ComponentInfo::next(COMPONENT component) {

    return COMPONENT (((int)component + 1) % COMP_MAX);
}

COMPONENT ComponentInfo::prev(COMPONENT component) {

    return COMPONENT (((int)component + COMP_MAX - 1) % COMP_MAX);
}
