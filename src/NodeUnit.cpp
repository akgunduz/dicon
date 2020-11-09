//
// Created by Haluk AKGUNDUZ on 01.08.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#include "NodeUnit.h"

NodeUnit::NodeUnit(NODESTATES _state, int _usage, ARCH _arch, long _id, TypeAddress _address)
        : NodeObject(_state, _usage), ComponentUnit(COMP_NODE, _arch, _id, std::move(_address)) {
}

NodeUnit::NodeUnit(ARCH _arch, long _id, TypeAddress _address)
        : ComponentUnit(COMP_NODE, _arch, _id, std::move(_address)) {
}

NodeUnit::NodeUnit(const NodeUnit &copy) = default;

NodeUnit::~NodeUnit() = default;