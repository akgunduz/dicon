//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#include "ComponentObject.h"

const char* sComponentTypes[COMP_MAX] = {
        "Distributor",
        "Collector",
        "Node",
};

ComponentObject::ComponentObject(COMPONENT _type, long _id, long _address)
        : ComponentInfo(_type, _id, _address)  {
}

ComponentObject::ComponentObject(COMPONENT _type, const char *_rootPath)
        : ComponentInfo(_type), rootPath(_rootPath) {
}

ComponentObject::ComponentObject(COMPONENT _type, const char* _rootPath, long _id, long _address)
        : ComponentInfo(_type, _id, _address), rootPath(_rootPath) {
}

ComponentObject::ComponentObject(const ComponentObject &copy)
        : ComponentInfo(copy), rootPath(copy.rootPath), assigned(copy.getAssigned()){
}

ComponentObject::ComponentObject(const ComponentObject &copy, COMPONENT target)
        : ComponentInfo(copy.getType(), copy.getID(), copy.getAddress(target)),
        rootPath(copy.rootPath), assigned(copy.getAssigned()) {
}


ComponentObject::~ComponentObject() = default;

const char* ComponentObject::getName() const {

    return sComponentTypes[type];
}

const char* ComponentObject::getRootPath() const {

    return rootPath;
}

void ComponentObject::setRootPath(const char *_rootPath) {

    this->rootPath = _rootPath;
}

const ComponentInfo &ComponentObject::getAssigned() const {

    return assigned;
}

void ComponentObject::setAssigned(COMPONENT type, long id, long address) {

    assigned.setType(type);
    assigned.setID(id);
    assigned.setAddress(address);
}
