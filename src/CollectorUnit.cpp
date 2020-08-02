//
// Created by Haluk AKGUNDUZ on 1.08.2020.
//

#include "CollectorUnit.h"

CollectorUnit::CollectorUnit(COLLSTATES _state, long _id, Address _address)
        : CollectorObject(_state), ComponentUnit(COMP_COLLECTOR, _id, _address) {
}

CollectorUnit::CollectorUnit(long _id, Address _address)
        : ComponentUnit(COMP_COLLECTOR, _id, _address) {
}

CollectorUnit::CollectorUnit()
        : ComponentUnit(COMP_COLLECTOR) {
}

CollectorUnit::CollectorUnit(const CollectorUnit &copy) = default;

CollectorUnit::~CollectorUnit() = default;