//
// Created by Haluk AKGUNDUZ on 14.08.2020.
//

#ifndef DICON_COMPONENTUNITFACTORY_H
#define DICON_COMPONENTUNITFACTORY_H

#include "ComponentType.h"
#include "Address.h"
#include "ComponentUnit.h"

class ComponentUnitFactory {
public:

    static ComponentUnit* create(COMPONENT, long, Address&);
};


#endif //DICON_COMPONENTUNITFACTORY_H
