//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#include "ComponentObject.h"

const char* sComponentTypes[COMP_MAX] = {
        "Distributor",
        "Collector",
        "Node",
};

ComponentObject::ComponentObject(COMPONENT type, const char* rootPath, int id)
        : type(type), id(id), rootPath(rootPath) {
}

ComponentObject::ComponentObject(const ComponentObject &copy)
        : type(copy.type), id(copy.id), rootPath(copy.rootPath) {
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

COMPONENT ComponentObject::getNextType() {

    return (COMPONENT)(((int)type + 1) % COMP_MAX);
}

COMPONENT ComponentObject::getPrevType() {

    return (COMPONENT)(((int)type + COMP_MAX - 1) % COMP_MAX);
}

const char* ComponentObject::getRootPath() {

    return rootPath;
}
