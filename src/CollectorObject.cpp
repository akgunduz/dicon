//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#include "CollectorObject.h"

CollectorObject::CollectorObject(COLLSTATES state, ComponentObject attachedNode, int id, long address) :
        state(state), attachedNode(attachedNode), ComponentObject(COMP_COLLECTOR, id, address) {
}

CollectorObject::CollectorObject(int id, long address) :
        CollectorObject(COLLSTATE_IDLE, ComponentObject(), id, address) {
}

CollectorObject::~CollectorObject() {

}

COLLSTATES CollectorObject::getState() {

    return state;
}

void CollectorObject::setState(COLLSTATES state) {

    this->state = state;
}

ComponentObject CollectorObject::getAttached() {

    return attachedNode;
}

void CollectorObject::setAttached(ComponentObject attachedNode) {

    this->attachedNode = attachedNode;
}

