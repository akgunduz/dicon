//
// Created by Haluk AKGUNDUZ on 1.08.2020.
//

#include "ComponentType.h"

const std::string ComponentType::sComponentTypes[COMP_MAX] = {
        "Distributor",
        "Collector",
        "Node",
};

const std::string& ComponentType::getName(COMPONENT _type) {

    return sComponentTypes[_type];
}
