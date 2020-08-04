//
// Created by Haluk AKGUNDUZ on 1.08.2020.
//

#ifndef DICON_COMPONENTOBJECT_H
#define DICON_COMPONENTOBJECT_H

#include "ComponentUnit.h"

class ComponentObject {

    ComponentUnit assigned{};

public:

    ComponentUnit& getAssigned();
    void setAssigned(COMPONENT, long, Address);

};

#endif //DICON_COMPONENTOBJECT_H
