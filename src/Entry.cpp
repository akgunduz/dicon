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

enum APPPARAM {
    APPPARAM_OK,
    APPPARAM_ERROR,
    APPPARAM_LIST,
};

void listDevices() {

    DeviceList *deviceList = DeviceList::getInstance();

    LOGP_I("Listing Interfaces .....");

    for (int j = 0; j < deviceList->getCount(); j++) {
        LOGP_I("%s : %s", CommInterfaceType::getName(deviceList->get(j)->getType()),
               deviceList->get(j)->getName().c_str());
    }
}

APPPARAM parseParameters(int argc, char** argv, int *interfaceID,
                     LOGLEVEL& logLevel, std::vector<int>& componentCount) {

    for (int i = 1; i < argc; i++) {

        if (!strcmp(argv[i], "-l")) {

            listDevices();

            return APPPARAM_LIST;

        } else if (!strcmp(argv[i], "-i")) {

            if (argc > i + 1) {

                if (isdigit(argv[++i][0])) {
                    interfaceID[0] = atoi(argv[i]);

                } else {
                    return APPPARAM_ERROR;
                }

                if (isdigit(argv[++i][0])) {
                    interfaceID[1] = atoi(argv[i]);

                } else {
                    return APPPARAM_ERROR;
                }
            }

        } else if (!strcmp(argv[i], "-d")) {

            componentCount[COMP_DISTRIBUTOR] = 1;

        } else if (!strcmp(argv[i], "-c")) {

            if (isdigit(argv[++i][0])) {
                componentCount[COMP_COLLECTOR] =  atoi(argv[i]);

            } else {

                return APPPARAM_ERROR;
            }

        } else if (!strcmp(argv[i], "-n")) {

            if (isdigit(argv[++i][0])) {
                componentCount[COMP_NODE] =  atoi(argv[i]);

            } else {

                return APPPARAM_ERROR;
            }

        } else if (!strcmp(argv[i], "-g")) {

            if (argc > i + 1) {

                if (isdigit(argv[++i][0])) {
                    logLevel = (LOGLEVEL)atoi(argv[i]);

                } else {
                    return APPPARAM_ERROR;
                }
            }
        }
    }

    return APPPARAM_OK;
}

int main(int argc, char** argv) {

    int interfaceID[2] = {0, 0};

    LOGLEVEL logLevel = LEVEL_DEBUG;

    std::vector<int> componentCount = {0, 0, 0};

    APPPARAM res = parseParameters(argc, argv, interfaceID, logLevel, componentCount);

    if (res != APPPARAM_OK) {

        if (res == APPPARAM_ERROR) {
            LOGP_E("Parameter problem, exiting.....");
        }

        return 0;
    }

    App *app = nullptr;

#if defined(CONSOLE)

    app = new ConsoleApp(interfaceID, logLevel, componentCount);

#elif defined(WEB)

    app = new WebApp(interfaceID, logLevel, componentCount);

#elif defined(TEST)

    app = new TestApp(interfaceID, logLevel, componentCount);

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
