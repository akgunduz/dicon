//
// Created by Haluk AKGUNDUZ on 01.08.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#include "CollectorUnit.h"

CollectorUnit::CollectorUnit(COLLSTATES _state, ARCH _arch, long _id, TypeAddress _address)
        : CollectorObject(_state), ComponentUnit(COMP_COLLECTOR, _arch, _id, std::move(_address)) {
}

CollectorUnit::CollectorUnit(ARCH _arch, long _id, TypeAddress _address)
        : ComponentUnit(COMP_COLLECTOR, _arch, _id, std::move(_address)) {
}

CollectorUnit::CollectorUnit(const CollectorUnit &copy) = default;

CollectorUnit::~CollectorUnit() = default;