//
// Created by Haluk AKGUNDUZ on 28.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "CollectorObject.h"

CollectorObject::CollectorObject(COLLSTATES _state) :
        state(_state) {
}

COLLSTATES CollectorObject::getState() {

    return state;
}

void CollectorObject::setState(COLLSTATES _state) {

    this->state = _state;
}


