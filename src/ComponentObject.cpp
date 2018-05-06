//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#include "ComponentObject.h"

const char* sComponentTypes[COMP_MAX] = {
        "Distributor",
        "Collector",
        "Node",
};

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

COMPONENT ComponentObject::getType() {

    return this->type;
}

int ComponentObject::getID() {

    return this->id;
}

void ComponentObject::setID(int id) {

    this->id = id;
}

const char* ComponentObject::getName() {

    return sComponentTypes[type];
}

long ComponentObject::getAddress() {

    return address;
}

void ComponentObject::setAddress(long address) {

    this->address = address;
}

COMPONENT ComponentObject::getNextType() {

    return (COMPONENT)(((int)type + 1) % COMP_MAX);
}

COMPONENT ComponentObject::getPrevType() {

    return (COMPONENT)(((int)type + COMP_MAX - 1) % COMP_MAX);
}

const char* ComponentObject::getRootPath() {

    return rootPath;
}

void ComponentObject::setRootPath(const char *rootPath) {

    this->rootPath = rootPath;
}






