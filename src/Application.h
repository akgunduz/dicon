//
// Created by Haluk AKGUNDUZ on 03.05.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_APPLICATION_H
#define DICON_APPLICATION_H

#include "ComponentFactory.h"
#include "DeviceList.h"
#include "Log.h"

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

    ComponentFactory* componentFactory;

    bool initialized{false};

    App(enum APPTYPE, int *, const LogInfo&, std::vector<int>, bool);

public:

    virtual int run() = 0;

    virtual int notifyHandler(COMPONENT, NOTIFYTYPE) = 0;

    virtual ~App();

    bool isInitialized();
};

#endif //DICON_APPLICATION_H
