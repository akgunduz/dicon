//
// Created by Haluk AKGUNDUZ on 14/04/2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//


#ifndef DICON_NODESTATE_H
#define DICON_NODESTATE_H

enum NODE_STATES {
    IDLE,
    PREBUSY,
    BUSY,
};

class NodeState {

public:
    static const char* getName(NODE_STATES);
};


#endif //DICON_NODESTATE_H
