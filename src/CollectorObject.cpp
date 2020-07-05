//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#include "CollectorObject.h"

CollectorObject::CollectorObject(COLLSTATES _state, int id, long address) :
        state(_state), ComponentObject(COMP_COLLECTOR, id, address) {
}

CollectorObject::CollectorObject(int id, long address) :
        CollectorObject(COLLSTATE_START, id, address) {
}

CollectorObject::CollectorObject(const char* rootPath) :
        ComponentObject(COMP_COLLECTOR, rootPath)  {
}

CollectorObject::CollectorObject() :
        CollectorObject(0, 0) {
}

CollectorObject::~CollectorObject() {

}

COLLSTATES CollectorObject::getState() {

    return state;
}

void CollectorObject::setState(COLLSTATES _state) {

    this->state = _state;
}
