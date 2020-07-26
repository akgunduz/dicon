//
// Created by Haluk AKGUNDUZ on 03.05.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_APPLICATION_H
#define DICON_APPLICATION_H

#include <ComponentController.h>

#define MAX_UI_CB 100

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

    App(enum APPTYPE, int *, LOGLEVEL*, std::vector<int>);

public:

    static bool notifyCB(void *, COMPONENT, long);

    virtual int run() = 0;

    virtual int notifyHandler(COMPONENT, long) = 0;

};



#endif //DICON_APPLICATION_H
