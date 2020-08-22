//
// Created by Haluk AKGUNDUZ on 1.08.2020.
//

#include "NodeUnit.h"

NodeUnit::NodeUnit(NODESTATES _state, int _usage, ARCH _arch, long _id, Address _address)
        : NodeObject(_state, _usage), ComponentUnit(COMP_NODE, _arch, _id, _address) {
}

NodeUnit::NodeUnit(ARCH _arch, long _id, Address _address)
        : ComponentUnit(COMP_NODE, _arch, _id, _address) {
}

NodeUnit::NodeUnit(const NodeUnit &copy) = default;

NodeUnit::~NodeUnit() = default;