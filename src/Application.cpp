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

int listDevices() {

    DeviceList *deviceList = DeviceList::getInstance();

    LOG_S("Listing Interfaces .....");

    for (int j = 0; j < deviceList->getCount(); j++) {
        LOG_S("%s : %s", InterfaceTypes::getName(deviceList->get(j)->getType()),
              deviceList->get(j)->getName());
    }

    return 0;
}

bool parseParameters(int argc, char** argv, int *interfaceID,
                     LOGLEVEL* logLevel, int* disCount, int* collInfo, int* nodeInfo) {

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

            *disCount = 1;

        } else if (!strcmp(argv[i], "-c")) {

            if (isdigit(argv[++i][0])) {
                collInfo[0] =  atoi(argv[i]);

            } else {

                return false;
            }

            if (isdigit(argv[i + 1][0])) {
                collInfo[1] =  atoi(argv[++i]);

            }

        } else if (!strcmp(argv[i], "-n")) {

            if (isdigit(argv[++i][0])) {
                nodeInfo[0] =  atoi(argv[i]);

            } else {

                return false;
            }

            if (isdigit(argv[i + 1][0])) {
                nodeInfo[1] =  atoi(argv[++i]);

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

    Util::cleanup();

    int interfaceID[2] = {0, 0};

    LOGLEVEL logLevel[2] = {LEVEL_TRACE, LEVEL_TRACE};

    int distCount = 0;
    int collInfo[2] = {0, 1};
    int nodeInfo[2] = {0, 1};

    if (!parseParameters(argc, argv, interfaceID, logLevel,
            &distCount, collInfo, nodeInfo)) {
        LOG_S("Parameter problem, exiting.....");
        return 0;
    }

    UserInterfaceApp *app = nullptr;

#if defined(WX_UI)

    distCount = 1;
    collInfo[0] = 1;
    collInfo[1] = 1;
    nodeInfo[0] = 1;
    nodeInfo[1] = 1;

    app = new WxApp(argc, argv, interfaceID, logLevel, &distCount, collInfo, nodeInfo);

#elif defined(CONSOLE_UI)

    app = new ConsoleApp(argc, argv, interfaceID, logLevel, &distCount, collInfo, nodeInfo);

#elif defined(WEB_UI)

    app = new WebApp(argc, argv, interfaceID, logLevel, &distCount, collInfo, nodeInfo);

#else

#error "Application Default mode is not selected"

#endif

    return app->run();
}
