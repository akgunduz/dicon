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
#include "Application.h"
#include "DeviceList.h"
#include "Util.h"
#include "ComponentController.h"

int main(int argc, char** argv) {

    Util::cleanup();

    int distCount = 0;
    int collCount = 0;
    int nodeCount = 0;
    int collIndex = 1;
    int nodeIndex = 1;

    int interfaceID[2] = {0, 0};

    void *app = NULL;

    LOGLEVEL logLevel[2] = {LEVEL_INFO, LEVEL_ERROR};

#if defined(WX_UI)

    APPMODE appMode = APPMODE::APPMODE_WXWIDGETS;

#elif defined(CONSOLE_UI)

    APPMODE appMode = APPMODE::APPMODE_CONSOLE;

#else

    APPMODE appMode = APPMODE::APPMODE_NOUI;

#endif

    DeviceList *deviceList = DeviceList::getInstance();

    for (int i = 1; i < argc; i++) {

        if (!strcmp(argv[i], "-l")) {

            LOG_S("Listing Interfaces .....");

            for (int j = 0; j < deviceList->getCount(); j++) {
                LOG_S("%s : %s", InterfaceTypes::getName(deviceList->get(j)->getType()),
                      deviceList->get(j)->getName());
            }

            return 0;

        } else if (!strcmp(argv[i], "-i")) {

            if (argc > i + 1) {

                if (isdigit(argv[++i][0])) {
                    interfaceID[0] = atoi(argv[i]);

                } else {
                    return 0;
                }

                if (isdigit(argv[++i][0])) {
                    interfaceID[1] = atoi(argv[i]);

                } else {
                    return 0;
                }
            }

        } else if (!strcmp(argv[i], "-s")) {

            appMode = APPMODE::APPMODE_NOUI;

        } else if (!strcmp(argv[i], "-d")) {

            if (isdigit(argv[++i][0])) {
                distCount =  std::min(atoi(argv[i]), 1);

            } else {

                return 0;
            }

        } else if (!strcmp(argv[i], "-c")) {

            if (isdigit(argv[++i][0])) {
                collCount =  atoi(argv[i]);

            } else {

                return 0;
            }

            if (isdigit(argv[i + 1][0])) {
                collIndex =  atoi(argv[++i]);

            }

        } else if (!strcmp(argv[i], "-n")) {

            if (isdigit(argv[++i][0])) {
                nodeCount =  atoi(argv[i]);

            } else {

                return 0;
            }

            if (isdigit(argv[i + 1][0])) {
                nodeIndex =  atoi(argv[++i]);

            }

        } else if (!strcmp(argv[i], "-g")) {

            if (argc > i + 1) {

                if (isdigit(argv[++i][0])) {
                    logLevel[0] = (LOGLEVEL)atoi(argv[i]);

                } else {
                    return 0;
                }

                if (isdigit(argv[++i][0])) {
                    logLevel[1] = (LOGLEVEL)atoi(argv[i]);

                } else {
                    return 0;
                }
            }
        }
    }

    if (distCount == 0 && collCount == 0 && nodeCount == 0) {

        distCount = 1;
        collCount = 1;
        nodeCount = 1;
    }

#ifdef WX_UI
    if (appMode == APPMODE::APPMODE_WXWIDGETS) {

//        distCount = std::min(distCount, 1);
//        collCount = std::min(collCount, 1);
//        nodeCount = std::min(nodeCount, 1);
        collIndex = 1;
        nodeIndex = 1;
    }
#endif

    Log::init(logLevel[0], logLevel[1]);

    LOG_S("Running in %s Mode with %d Distributor, %d Collector and %d Node, using interfaces : %s and %s",
          appMode == APPMODE::APPMODE_NOUI ? "No UI" : appMode == APPMODE::APPMODE_CONSOLE ? "Console" : "WxWidgets",
          distCount, collCount, nodeCount,
          deviceList->get(interfaceID[0])->getName(), deviceList->get(interfaceID[1])->getName());

    ComponentController *controller = ComponentController::newInstance(interfaceID[0], interfaceID[1]);

    if (distCount) {
        controller->startDistributor();
    }

    if (collCount) {
        controller->startCollector(collCount, collIndex);
    }

    if (nodeCount) {
        controller->startNode(nodeCount, nodeIndex);
    }

#ifdef WX_UI
    if (appMode == APPMODE::APPMODE_WXWIDGETS) {

        app = new WxApp(controller);

        wxApp::SetInstance((WxApp*)app);
        return wxEntry(argc, argv);
    }
#endif

#ifdef CONSOLE_UI
    if (appMode == APPMODE::APPMODE_CONSOLE) {

        app = new ConsoleApp(controller);

    }
#endif

    int in;
    do {

        in = getchar();
        switch(in) {
            case 'p':
                if (controller->getDistributorCount()) {
                    controller->getDistributor()->sendWakeupMessagesAll(false);
                }
                break;
            case 'l':
                if (controller->getCollectorCount()) {
                    controller->getCollector(0)->loadJob(NULL);
                }
                break;
            case 'x':
                if (controller->getCollectorCount()) {
                    controller->getCollector(0)->processJobs();
                }
                break;
            case 'q':
                delete controller;
#ifdef CONSOLE_UI
                if (appMode == APPMODE::APPMODE_CONSOLE) {
                    delete ((ConsoleApp*)app);
                }
#endif
                return 0;
            default:
                break;
        }

    } while(true);
}
