//
// Created by Haluk AKGUNDUZ on 14/04/2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_COLLECTORSTATE_H
#define DICON_COLLECTORSTATE_H

enum COLLSTATES {
    COLLSTATE_IDLE,
    COLLSTATE_BUSY,
    COLLSTATE_MAX
};

class CollState {

public:
    static const char* getName(COLLSTATES);
};

#endif //DICON_COLLECTORSTATE_H
