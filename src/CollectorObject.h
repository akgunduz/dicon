//
// Created by Haluk AKGUNDUZ on 28.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_COLLECTOROBJECT_H
#define DICON_COLLECTOROBJECT_H

#include "ComponentObject.h"
#include "CollectorState.h"

class CollectorObject : public ComponentObject {

    COLLSTATES state{COLLSTATE_IDLE};

public:

    CollectorObject() = default;
    explicit CollectorObject(COLLSTATES);

    COLLSTATES getState();
    void setState(COLLSTATES);

};

#endif //DICON_COLLECTOROBJECT_H
