//
// Created by Haluk AKGUNDUZ on 14/04/2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//


#ifndef BANKOR_NODESTATE_H
#define BANKOR_NODESTATE_H

enum NODE_STATES {
    IDLE,
    BUSY,
    PREBUSY,
    REMOVE
};

class NodeState {

public:
    static const char* getName(NODE_STATES);
};


#endif //BANKOR_NODESTATE_H
