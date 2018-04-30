//
// Created by Haluk AKGUNDUZ on 29/03/18.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "ComponentTypes.h"

const char* sComponentTypes[COMP_MAX] = {
        "Distributor",
        "Collector",
        "Node",
};

char ComponentTypes::rootPath[COMP_MAX][PATH_MAX];

const char* ComponentTypes::getName(COMPONENT type) {

    return sComponentTypes[type];
}

COMPONENT ComponentTypes::getNextType(COMPONENT type) {

    return (COMPONENT)(((int)type + 1) % COMP_MAX);
}

COMPONENT ComponentTypes::getPrevType(COMPONENT type) {

    return (COMPONENT)(((int)type + COMP_MAX - 1) % COMP_MAX);
}

const char* ComponentTypes::getRootPath(COMPONENT component) {
    return rootPath[component];
}

void ComponentTypes::setRootPath(COMPONENT component, const char *path) {
    strcpy(rootPath[component], path);
}
