//
// Created by Haluk AKGUNDUZ on 1.08.2020.
//

#include "CollectorHost.h"

CollectorHost::CollectorHost(COLLSTATES _state, long _id)
        : CollectorObject(_state), HostUnit(COMP_COLLECTOR, _id) {
}

CollectorHost::CollectorHost(long _id)
        : HostUnit(COMP_COLLECTOR, _id) {
}

CollectorHost::CollectorHost()
        : HostUnit(COMP_COLLECTOR) {
}

CollectorHost::CollectorHost(const char* rootPath) :
        HostUnit(COMP_COLLECTOR, rootPath)  {
}

CollectorHost::CollectorHost(const CollectorHost &copy) = default;

CollectorHost::~CollectorHost() = default;