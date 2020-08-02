//
// Created by Haluk Akgunduz on 10/18/15.
//

#include "NodeHost.h"
#include "ProcessItem.h"

NodeHost::NodeHost(NODESTATES _state, int _usage, long _id)
        : NodeObject(_state, _usage), HostUnit(COMP_NODE, _id) {
}

NodeHost::NodeHost(long _id)
        : HostUnit(COMP_NODE, _id) {
}

NodeHost::NodeHost()
        : HostUnit(COMP_NODE) {
}

NodeHost::NodeHost(const char* rootPath) :
        HostUnit(COMP_NODE, rootPath)  {
}

NodeHost::NodeHost(const NodeHost &copy) = default;

NodeHost::~NodeHost() = default;

ProcessItem &NodeHost::getProcess() {

    return processItem;
}
