//
// Created by Haluk AKGUNDUZ on 1.08.2020.
//

#include "CollectorUnit.h"

CollectorUnit::CollectorUnit(COLLSTATES _state, ARCH _arch, long _id, Address _address)
        : CollectorObject(_state), ComponentUnit(COMP_COLLECTOR, _arch, _id, _address) {
}

CollectorUnit::CollectorUnit(ARCH _arch, long _id, Address _address)
        : ComponentUnit(COMP_COLLECTOR, _arch, _id, _address) {
}

CollectorUnit::CollectorUnit()
        : ComponentUnit(COMP_COLLECTOR) {
}

CollectorUnit::CollectorUnit(ComponentUnit& copy)
        : ComponentUnit(copy.getType(), copy.getArch(), copy.getID(), copy.getAddress()) {
}

CollectorUnit::CollectorUnit(const CollectorUnit &copy) = default;

CollectorUnit::~CollectorUnit() = default;