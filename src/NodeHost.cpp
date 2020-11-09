//
// Created by Haluk AKGUNDUZ on 18.10.2015.
// Copyright (c) 2015 Haluk Akgunduz. All rights reserved.
//

#include "NodeHost.h"

NodeHost::NodeHost(long _id, NODESTATES _state, int _usage)
        : NodeObject(_state, _usage), HostUnit(COMP_NODE, _id) {

}

NodeHost::NodeHost(const NodeHost &copy) = default;

NodeHost::~NodeHost() = default;
