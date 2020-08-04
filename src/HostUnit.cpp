//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#include "HostUnit.h"

HostUnit::HostUnit(COMPONENT _type, const char *_rootPath)
        : type(_type), rootPath(_rootPath) {
}

HostUnit::HostUnit(COMPONENT _type, long _id)
        : type(_type), id(_id)  {
}

HostUnit::HostUnit(COMPONENT _type, long _id, const char* _rootPath)
        : type(_type), id(_id), rootPath(_rootPath) {
}

HostUnit::HostUnit(const HostUnit &copy) = default;

HostUnit::~HostUnit() = default;

const char* HostUnit::getRootPath() const {

    return rootPath;
}

void HostUnit::setRootPath(const char *_rootPath) {

    rootPath = _rootPath;
}

long HostUnit::getID() const {

    return id;
}

void HostUnit::setID(long _id) {

    id = _id;
}

COMPONENT HostUnit::getType() const {

    return type;
}

void HostUnit::setType(COMPONENT _type) {

    type = _type;
}

Address &HostUnit::getAddress(COMPONENT _out) {

    assert(_out != type);
    return address[_out];
}

void HostUnit::setAddress(COMPONENT _out, Address _address) {

    assert(_out != type);
    address[_out] = _address;
}

void HostUnit::set(COMPONENT _type, long _id, COMPONENT _out, Address _address) {

    assert(_out != _type);
    type = _type;
    id = _id;
    address[_out] = _address;
}

ComponentUnit HostUnit::getUnit(COMPONENT targetType) {

    return ComponentUnit(type, id, address[targetType]);
}
