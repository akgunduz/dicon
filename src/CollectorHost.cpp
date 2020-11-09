//
// Created by Haluk AKGUNDUZ on 01.08.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#include "CollectorHost.h"

CollectorHost::CollectorHost(long _id, COLLSTATES _state)
        : CollectorObject(_state), HostUnit(COMP_COLLECTOR, _id) {
}

CollectorHost::CollectorHost(const CollectorHost &copy) = default;

CollectorHost::~CollectorHost() = default;