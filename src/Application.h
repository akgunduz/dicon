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
    APPTYPE_WXWIDGETS,
    APPTYPE_WEB,
    APPTYPE_MAX
};

class App {

    static App* instance;

    DeviceList *deviceList;

    enum APPTYPE type;

protected:

    int argc;
    char** argv;

    ComponentController* componentController;

    App(enum APPTYPE mode, int argc, char** argv, int *interfaceID,
                     LOGLEVEL* logLevel, bool enableDistributor, int* collInfo, int* nodeInfo);

public:

    static bool notifyCB(void *, int, int);

    enum APPTYPE getAppType();

    virtual int run() = 0;

    virtual int notifyHandler(int, int) = 0;

};

#endif //DICON_APPLICATION_H
