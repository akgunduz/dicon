//
// Created by Haluk AKGUNDUZ on 03.05.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifdef WX_UI
#include <ui/wx/WxApp.h>
#endif
#ifdef CONSOLE_UI
#include <ui/console/ConsoleApp.h>
#endif
#ifdef WEB_UI
#include <ui/web/WebApp.h>
#endif

#include "Application.h"

int listDevices() {

    DeviceList *deviceList = DeviceList::getInstance();

    PRINT("Listing Interfaces .....");

    for (int j = 0; j < deviceList->getCount(); j++) {
        PRINT("%s : %s", InterfaceTypes::getName(deviceList->get(j)->getType()),
              deviceList->get(j)->getName());
    }

    return 0;
}

bool parseParameters(int argc, char** argv, int *interfaceID,
                     LOGLEVEL* logLevel, int* componentCount) {

    for (int i = 1; i < argc; i++) {

        if (!strcmp(argv[i], "-l")) {

            return listDevices();

        } else if (!strcmp(argv[i], "-i")) {

            if (argc > i + 1) {

                if (isdigit(argv[++i][0])) {
                    interfaceID[0] = atoi(argv[i]);

                } else {
                    return false;
                }

                if (isdigit(argv[++i][0])) {
                    interfaceID[1] = atoi(argv[i]);

                } else {
                    return false;
                }
            }

        } else if (!strcmp(argv[i], "-d")) {

            componentCount[COMP_DISTRIBUTOR] = 1;

        } else if (!strcmp(argv[i], "-c")) {

            if (isdigit(argv[++i][0])) {
                componentCount[COMP_COLLECTOR] =  atoi(argv[i]);

            } else {

                return false;
            }

        } else if (!strcmp(argv[i], "-n")) {

            if (isdigit(argv[++i][0])) {
                componentCount[COMP_NODE] =  atoi(argv[i]);

            } else {

                return false;
            }

        } else if (!strcmp(argv[i], "-g")) {

            if (argc > i + 1) {

                if (isdigit(argv[++i][0])) {
                    logLevel[0] = (LOGLEVEL)atoi(argv[i]);

                } else {
                    return false;
                }

                if (isdigit(argv[++i][0])) {
                    logLevel[1] = (LOGLEVEL)atoi(argv[i]);

                } else {
                    return false;
                }
            }
        }
    }

    return true;
}

int main(int argc, char** argv) {

    Util::init();

    int interfaceID[2] = {0, 0};

    LOGLEVEL logLevel[2] = {LEVEL_INFO, LEVEL_ERROR};

    int componentCount[3] = {0, 0, 0};

    if (!parseParameters(argc, argv, interfaceID, logLevel,
                         componentCount)) {
        PRINT("Parameter problem, exiting.....");
        return 0;
    }

    App *app = nullptr;

#if defined(CONSOLE_UI)

    app = new ConsoleApp(argc, argv, interfaceID, logLevel, componentCount);

#elif defined(WEB_UI)

    app = new WebApp(argc, argv, interfaceID, logLevel, componentCount);

#else

#error "Application Default mode is not selected"

#endif

    return app->run();
}
