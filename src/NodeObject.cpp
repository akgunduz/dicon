//
// Created by Haluk Akgunduz on 10/18/15.
//

#include "NodeObject.h"

NodeObject::NodeObject(NODE_STATES state, int usage, long address) :
        state(state), usage(usage), address(address) {

}

NodeObject::NodeObject(long address) :
        state(IDLE), usage(0), address(address) {
}

NodeObject::~NodeObject() {

#ifndef DISABLE_RECOVERY
    delete watchdog;
#endif

}

NODE_STATES NodeObject::getState() {

    return state;
}

int NodeObject::getUsage() {

    return usage;
}

void NodeObject::iterateUsage(bool direction) {

    direction ? usage++ : usage--;
}

long NodeObject::getAddress() {

    return address;
}

std::vector<Md5> *NodeObject::getMD5List() {

    return &md5List;
}

void NodeObject::setState(NODE_STATES state) {

    this->state = state;
}


