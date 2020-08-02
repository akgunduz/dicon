//
// Created by Haluk AKGUNDUZ on 28.04.2018.
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


