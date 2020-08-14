//
// Created by Haluk AKGUNDUZ on 14.08.2020.
//

#include "ComponentUnitFactory.h"
#include "NodeUnit.h"
#include "CollectorUnit.h"

ComponentUnit *ComponentUnitFactory::create(COMPONENT type, long id, Address &address) {

    switch(type) {
        case COMP_COLLECTOR:
            return new CollectorUnit(id, address);
        case COMP_NODE:
            return new NodeUnit(id, address);
        default:
            break;
    }
    return nullptr;
}
