//
// Created by Haluk Akgunduz on 10/18/15.
//

#include "NodeObject.h"

NodeObject::NodeObject(NODE_STATES state, int usage) :
        state(state), usage(usage) {

}

NodeObject::NodeObject() :
        state(IDLE), usage(0) {
}

NodeObject::~NodeObject() {

}

NODE_STATES NodeObject::getState() {

    return state;
}

int NodeObject::getUsage() {

    return usage;
}

int NodeObject::iterateUsage(bool direction) {

    return direction ? ++usage : --usage;
}

std::vector<Md5> *NodeObject::getMD5List() {

    return &md5List;
}

void NodeObject::setState(NODE_STATES state) {

    this->state = state;
}


