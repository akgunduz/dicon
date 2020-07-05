//
// Created by Haluk Akgunduz on 10/18/15.
//

#include "NodeObject.h"

NodeObject::NodeObject(NODESTATES _state, int _usage, int id, long address) :
        state(_state), usage(_usage), ComponentObject(COMP_NODE, id, address) {
}

NodeObject::NodeObject(int id, long address) :
        NodeObject(NODESTATE_START, 0, id, address) {
}

NodeObject::NodeObject(const char* rootPath) :
        ComponentObject(COMP_NODE, rootPath)  {
}

NodeObject::NodeObject() :
        NodeObject(0, 0) {
}

NodeObject::NodeObject(const NodeObject &copy) :
        NodeObject(copy.state, copy.usage, copy.getID(), copy.getAddress()) {
}

NodeObject::NodeObject(const ComponentObject &copy) :
        NodeObject(copy.getID(), copy.getAddress()) {
}

NodeObject::~NodeObject() {

}

NODESTATES NodeObject::getState() {

    return state;
}

void NodeObject::setState(NODESTATES _state) {

    this->state = _state;
}

int NodeObject::getUsage() {

    return usage;
}

int NodeObject::iterateUsage(bool direction) {

    return direction ? ++usage : --usage;
}

//NodeProcessInfo &NodeObject::getProcess() {
//
//    return processInfo;
//}
