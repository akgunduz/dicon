//
// Created by Haluk AKGUNDUZ on 14/04/2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_NODESTATE_H
#define DICON_NODESTATE_H

enum NODESTATES {
    NODESTATE_IDLE,
    NODESTATE_PREBUSY,
    NODESTATE_BUSY,
    NODESTATE_MAX
};

class NodeState {

public:
    static const char* getName(NODESTATES);
};

#endif //DICON_NODESTATE_H
