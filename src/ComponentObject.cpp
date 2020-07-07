//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#include "ComponentObject.h"

const char* sComponentTypes[COMP_MAX] = {
        "Distributor",
        "Collector",
        "Node",
};

ComponentObject::ComponentObject()
        : ComponentObject(COMP_MAX, 0) {
}

ComponentObject::ComponentObject(COMPONENT type, int id)
        : ComponentObject(type, id, 0) {
}

ComponentObject::ComponentObject(COMPONENT type, int id, long address)
        : ComponentObject(type, "", id, address) {
}

ComponentObject::ComponentObject(COMPONENT type, const char *rootPath)
        : ComponentObject(type, rootPath, 0, 0) {
}

ComponentObject::ComponentObject(const ComponentObject &copy)
        : ComponentObject(copy.type, copy.rootPath, copy.getID(), copy.getAddress()) {
}

ComponentObject::ComponentObject(COMPONENT type, const char* rootPath, int id, long address)
        : type(type), rootPath(rootPath), assigned(0, 0), ComponentInfo(id, address) {
}

ComponentObject::~ComponentObject() = default;

COMPONENT ComponentObject::getType() const {

    return this->type;
}

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

void ComponentObject::setAssigned(int id, long address) {

    assigned.setID(id);
    assigned.setAddress(address);
}
