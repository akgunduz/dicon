//
// Created by Haluk AKGUNDUZ on 03.05.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_APPLICATION_H
#define DICON_APPLICATION_H

#include "ComponentController.h"

enum APPTYPE {

    APPTYPE_CONSOLE,
    APPTYPE_WEB,
    APPTYPE_TEST,
    APPTYPE_MAX
};

class App {

    DeviceList *deviceList;

    enum APPTYPE type;

protected:

    ComponentController* componentController;

    App(enum APPTYPE, int *, LOGLEVEL*, std::vector<int>, bool);

public:

    static bool notifyCB(void *, COMPONENT, NOTIFYSTATE);

    virtual int run() = 0;

    virtual int notifyHandler(COMPONENT, NOTIFYSTATE) = 0;

};



#endif //DICON_APPLICATION_H
