//
// Created by Haluk AKGUNDUZ on 24.06.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_USERINTERFACEAPP_H
#define DICON_USERINTERFACEAPP_H

#include <ComponentController.h>

#define MAX_UI_CB 100

enum APPTYPE {

    APPTYPE_CONSOLE,
    APPTYPE_WXWIDGETS,
    APPTYPE_WEB,
    APPTYPE_MAX
};

class UserInterfaceApp {

    DeviceList *deviceList;

    enum APPTYPE type;

protected:

    int argc;
    char** argv;

    ComponentController* componentController;

    UserInterfaceApp(enum APPTYPE mode, int argc, char** argv, int *interfaceID,
                              LOGLEVEL* logLevel, const int* distCount, int* collInfo, int* nodeInfo);

public:

    virtual int run() = 0;

    enum APPTYPE getAppType();
};


#endif //DICON_USERINTERFACEAPP_H
