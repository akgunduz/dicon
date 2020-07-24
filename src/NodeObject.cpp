//
// Created by Haluk Akgunduz on 10/18/15.
//

#include "NodeObject.h"
#include "ProcessItem.h"

NodeObject::NodeObject(NODESTATES _state, int _usage, long id, long address) :
        state(_state), usage(_usage), ComponentObject(COMP_NODE, id, address) {
}

NodeObject::NodeObject(long id, long address) :
        ComponentObject(COMP_NODE, id, address) {
}

NodeObject::NodeObject(const char* rootPath) :
        ComponentObject(COMP_NODE, rootPath)  {
}

NodeObject::NodeObject() :
        NodeObject(0, 0) {
}

NodeObject::NodeObject(const NodeObject &copy) :
        state(copy.state), usage(copy.usage), processItem(copy.processItem),
        ComponentObject(COMP_NODE, copy.getID(), copy.getAddress()) {
}

NodeObject::NodeObject(const ComponentObject &copy) :
        NodeObject(copy.getID(), copy.getAddress()) {
}

NodeObject::~NodeObject() = default;

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

ProcessItem &NodeObject::getProcess() {

    return processItem;
}
