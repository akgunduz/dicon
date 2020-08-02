//
// Created by Haluk AKGUNDUZ on 1.08.2020.
//

#include "ComponentType.h"

const char* sComponentTypes[COMP_MAX] = {
        "Distributor",
        "Collector",
        "Node",
};

const char *ComponentType::getName(COMPONENT _type) {

    return sComponentTypes[_type];
}
