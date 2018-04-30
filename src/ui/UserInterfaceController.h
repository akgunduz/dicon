//
// Created by Haluk AKGUNDUZ on 30.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef BANKOR_USERINTERFACECONTROLLER_H
#define BANKOR_USERINTERFACECONTROLLER_H

#include "UserInterfaceEvent.h"

class UserInterfaceController;

typedef void (UserInterfaceController::*fUIUpdater)(UserInterfaceEvent &event);

class UserInterfaceController {

    fUIUpdater *uiUpdater;

public:

    static void updateUICallback(void*, int, void*);
    virtual void updateUIEvent(int, void *) = 0;
};


#endif //BANKOR_USERINTERFACECONTROLLER_H
