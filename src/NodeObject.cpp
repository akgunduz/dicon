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

NodeObject::NodeObject(const char* rootPath) :
        ComponentObject(COMP_NODE, rootPath)  {
}

NodeObject::NodeObject(const NodeObject &copy)
        : state(copy.state), usage(copy.usage),
        ComponentObject(copy.type, copy.rootPath, copy.id, copy.address) {
}

NodeObject::NodeObject(const ComponentObject &copy)
        : state(NODESTATE_IDLE), usage(0),
          ComponentObject(copy.type, copy.rootPath, copy.id, copy.address) {
}

NodeObject::NodeObject() :
        NodeObject(NODESTATE_IDLE, 0, 0, 0) {
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

void NodeObject::setState(NODESTATES _state) {

    this->state = _state;
}

NodeProcessInfo &NodeObject::getProcess() {

    return processInfo;
}
