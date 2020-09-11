//
// Created by Haluk Akgunduz on 10/18/15.
//

#include "NodeHost.h"
#include "ProcessItem.h"

NodeHost::NodeHost(long _id, NODESTATES _state, int _usage)
        : NodeObject(_state, _usage), Host(COMP_NODE, _id) {

}

//NodeHost::NodeHost(const NodeHost &copy) = default;

NodeHost::~NodeHost() = default;
