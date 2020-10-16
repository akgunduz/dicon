//
// Created by Haluk AKGUNDUZ on 03.05.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#if defined(CONSOLE)
#include <ui/console/ConsoleApp.h>
#elif defined(WEB)
#include <ui/web/WebApp.h>
#elif defined(TEST)
#include <test/TestApp.h>
#endif

#include "DeviceList.h"
#include "UtilNet.h"
#include "Log.h"

void listDevices() {

    DeviceList *deviceList = DeviceList::getInstance();

    LOGP_I("Listing Interfaces .....");

    for (size_t j = 0; j < deviceList->getCount(); j++) {
        LOGP_I("%ld -> %s : %s", j, CommInterfaceType::getName(deviceList->get(j)->getType()),
               deviceList->get(j)->getName().c_str());
    }
}

APPPARAM parseParameters(int argc, char** argv, AppParams& params) {

    for (int i = 1; i < argc; i++) {

        if (!strcmp(argv[i], "-l")) {

            listDevices();

            return APPPARAM_LIST;

        } else if (!strcmp(argv[i], "-i")) {

            if (argc > i + 1) {

                if (isdigit(argv[++i][0])) {
                    params.interfaceID[0] = atoi(argv[i]);

                } else {
                    return APPPARAM_ERROR;
                }

                if (isdigit(argv[++i][0])) {
                    params.interfaceID[1] = atoi(argv[i]);

                } else {
                    return APPPARAM_ERROR;
                }
            }

        } else if (!strcmp(argv[i], "-x")) {

            UtilNet::cleanUnixPath();

        } else if (!strcmp(argv[i], "-t")) {

            params.logInfo.timeStamp = true;

        } else if (!strcmp(argv[i], "-w")) {

            params.autoWake = false;

        } else if (!strcmp(argv[i], "-d")) {

            params.componentCount[COMP_DISTRIBUTOR] = 1;

        } else if (!strcmp(argv[i], "-c")) {

            if (isdigit(argv[++i][0])) {
                params.componentCount[COMP_COLLECTOR] =  atoi(argv[i]);

            } else {

                return APPPARAM_ERROR;
            }

        } else if (!strcmp(argv[i], "-n")) {

            if (isdigit(argv[++i][0])) {
                params.componentCount[COMP_NODE] =  atoi(argv[i]);

            } else {

                return APPPARAM_ERROR;
            }

        } else if (!strcmp(argv[i], "-g")) {

            if (argc > i + 1) {

                if (isdigit(argv[++i][0])) {
                    params.logInfo.level = (LOGLEVEL)atoi(argv[i]);

                } else {
                    return APPPARAM_ERROR;
                }
            }
        }
    }

    return APPPARAM_OK;
}

int main(int argc, char** argv) {

    AppParams params;

    APPPARAM res = parseParameters(argc, argv, params);

    if (res != APPPARAM_OK) {

        if (res == APPPARAM_ERROR) {
            LOGP_E("Parameter problem, exiting.....");
        }

        return 0;
    }

#if defined(CONSOLE)

    App *app = new ConsoleApp(params);

#elif defined(WEB)

    App *app = new WebApp(params);

#elif defined(TEST)

    App *app = new TestApp(params);

#else

#error "Application Default mode is not selected"

#endif

    int status = EXIT_FAILURE;
    if (app->isInitialized()) {
        status = app->run();
    }

    delete app;

    return status;
}
