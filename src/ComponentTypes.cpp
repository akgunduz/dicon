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

UI_UPDATE ComponentTypes::getAssignedUILog(COMPONENT type) {

    switch (type) {

        case COMP_DISTRIBUTOR:
            return UI_UPDATE_DIST_LOG;

        case COMP_COLLECTOR:
            return UI_UPDATE_COLL_LOG;

        case COMP_NODE:
            return UI_UPDATE_NODE_LOG;

        default:
            break;
    }

    return UI_UPDATE_LOG;
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
