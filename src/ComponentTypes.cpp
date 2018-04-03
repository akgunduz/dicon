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
