//
// Created by Haluk AKGUNDUZ on 03.05.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_APPLICATION_H
#define DICON_APPLICATION_H

#include <vector>
#include "ComponentType.h"
#include "NotifyType.h"
#include "Log.h"

struct LogInfo;
class ComponentFactory;
class DeviceList;

enum APPTYPE {

    APPTYPE_CONSOLE,
    APPTYPE_WEB,
    APPTYPE_TEST,
    APPTYPE_MAX
};

enum APPPARAM {
    APPPARAM_OK,
    APPPARAM_ERROR,
    APPPARAM_LIST,
};

struct AppParams {

    int interfaceID[2] {0, 0};

    LogInfo logInfo {LEVEL_INFO, LOG_ALL};

    std::vector<int> componentCount {0, 0, 0};

    bool autoWake {true};

};

class App {

    DeviceList *deviceList;

    enum APPTYPE type;

protected:

    ComponentFactory* componentFactory;

    bool initialized{false};

    App(enum APPTYPE, AppParams&);

public:

    virtual int run() = 0;

    virtual int notifyHandler(COMPONENT, NOTIFYTYPE) = 0;

    virtual ~App();

    bool isInitialized();
};

#endif //DICON_APPLICATION_H
