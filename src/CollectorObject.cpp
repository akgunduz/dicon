//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#include "CollectorObject.h"

CollectorObject::CollectorObject(COLLSTATES state, NodeObject attachedNode, int id, long address) :
        state(state), attachedNode(attachedNode), ComponentObject(COMP_COLLECTOR, id, address) {
}

CollectorObject::CollectorObject(int id, long address) :
        CollectorObject(COLLSTATE_IDLE, NodeObject(), id, address) {
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

void CollectorObject::setState(COLLSTATES state) {

    this->state = state;
}

NodeObject CollectorObject::getAttached() {

    return attachedNode;
}

void CollectorObject::setAttached(NodeObject attachedNode) {

    this->attachedNode = attachedNode;
}

