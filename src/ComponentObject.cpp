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
        : ComponentObject(copy.type, copy.rootPath, copy.id, copy.address) {
}

ComponentObject::ComponentObject(COMPONENT type, const char* rootPath, int id, long address)
        : type(type), id(id), rootPath(rootPath), address(address) {
}

ComponentObject::~ComponentObject() {

}

const COMPONENT ComponentObject::getType() const {

    return this->type;
}

const int ComponentObject::getID() const {

    return this->id;
}

void ComponentObject::setID(int _id) {

    this->id = _id;
}

const char* ComponentObject::getName() const {

    return sComponentTypes[type];
}

const long ComponentObject::getAddress() const {

    return address;
}

void ComponentObject::setAddress(long _address) {

    this->address = _address;
}

const char* ComponentObject::getRootPath() const {

    return rootPath;
}

void ComponentObject::setRootPath(const char *_rootPath) {

    this->rootPath = _rootPath;
}
