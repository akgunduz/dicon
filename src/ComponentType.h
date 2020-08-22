//
// Created by Haluk AKGUNDUZ on 1.08.2020.
//

#ifndef DICON_COMPONENTTYPE_H
#define DICON_COMPONENTTYPE_H

#include "Common.h"

enum COMPONENT {

    COMP_DISTRIBUTOR,
    COMP_COLLECTOR,
    COMP_NODE,
    COMP_MAX
};

class ComponentType {

    static const std::string sComponentTypes[COMP_MAX];

public:
    static const std::string& getName(COMPONENT);

};

#endif //DICON_COMPONENTTYPE_H
