//
// Created by akgun on 7.07.2020.
//

#include "ComponentInfo.h"

ComponentInfo::ComponentInfo() :
    id(0), address(0) {
}

ComponentInfo::ComponentInfo(int _id, long _address) :
    id(_id), address(_address) {
}

int ComponentInfo::getID() const {

    return this->id;
}

void ComponentInfo::setID(int _id) {

    this->id = _id;
}

long ComponentInfo::getAddress() const {

    return address;
}

void ComponentInfo::setAddress(long _address) {

    this->address = _address;
}
