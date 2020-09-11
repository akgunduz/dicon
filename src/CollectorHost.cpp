//
// Created by Haluk AKGUNDUZ on 1.08.2020.
//

#include "CollectorHost.h"

CollectorHost::CollectorHost(long _id, COLLSTATES _state)
        : CollectorObject(_state), Host(COMP_COLLECTOR, _id) {
}

//CollectorHost::CollectorHost(const CollectorHost &copy) = default;

CollectorHost::~CollectorHost() = default;