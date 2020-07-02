//
// Created by Haluk Akgunduz on 10/18/15.
//

#include "NodeObject.h"

NodeObject::NodeObject(NODESTATES state, int usage, int id, long address) :
        state(state), usage(usage), ComponentObject(COMP_NODE, id, address) {
}

NodeObject::NodeObject(int id, long address) :
        NodeObject(NODESTATE_IDLE, 0, id, address) {
}

NodeObject::~NodeObject() {

}

NODESTATES NodeObject::getState() {

    return state;
}

int NodeObject::getUsage() {

    return usage;
}

int NodeObject::iterateUsage(bool direction) {

    return direction ? ++usage : --usage;
}

void NodeObject::setState(NODESTATES state) {

    this->state = state;
}
