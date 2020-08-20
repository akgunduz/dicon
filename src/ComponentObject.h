//
// Created by Haluk AKGUNDUZ on 1.08.2020.
//

#ifndef DICON_COMPONENTOBJECT_H
#define DICON_COMPONENTOBJECT_H

#include "ComponentUnit.h"

class ComponentObject {

    TypeComponentUnit assigned{};

public:

    TypeComponentUnit& getAssigned();
    void setAssigned(COMPONENT, ARCH, long, Address);
    void setAssigned(const TypeComponentUnit&);

};

#endif //DICON_COMPONENTOBJECT_H
