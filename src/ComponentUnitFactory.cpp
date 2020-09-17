//
// Created by Haluk AKGUNDUZ on 14.08.2020.
//

#include "ComponentUnitFactory.h"
#include "NodeUnit.h"
#include "CollectorUnit.h"

TypeComponentUnit ComponentUnitFactory::create(COMPONENT type, ARCH arch, long id, TypeAddress& address) {

    switch(type) {

        case COMP_COLLECTOR:
            return std::make_shared<CollectorUnit>(arch, id, address);

        case COMP_NODE:
            return std::make_shared<NodeUnit>(arch, id, address);

        default:
            break;
    }
    return nullptr;
}
