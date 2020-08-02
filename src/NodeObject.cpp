//
// Created by Haluk AKGUNDUZ on 1.08.2020.
//

#include "NodeObject.h"

NodeObject::NodeObject(NODESTATES _state, int _usage)
        : state(_state), usage(_usage) {
}

NODESTATES NodeObject::getState() {

    return state;
}

void NodeObject::setState(NODESTATES _state) {

    this->state = _state;
}

int NodeObject::getUsage() const {

    return usage;
}

int NodeObject::iterateUsage(bool direction) {

    return direction ? ++usage : --usage;
}

