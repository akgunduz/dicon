//
// Created by Haluk AKGUNDUZ on 1.08.2020.
//

#include "NodeUnit.h"

NodeUnit::NodeUnit(NODESTATES _state, int _usage, long _id, Address _address)
        : NodeObject(_state, _usage), ComponentUnit(COMP_NODE, _id, _address) {
}

NodeUnit::NodeUnit(long _id, Address _address)
        : ComponentUnit(COMP_NODE, _id, _address) {
}

NodeUnit::NodeUnit()
        : ComponentUnit(COMP_NODE) {
}

NodeUnit::NodeUnit(const NodeUnit &copy) = default;

NodeUnit::~NodeUnit() = default;