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

    Log::init(LEVEL_INFO);

    Util::cleanup();

    int distCount = 0;
    int collCount = 0;
    int nodeCount = 0;

    int interfaceID[2] = {0, 0};

    void *app = NULL;

    APPMODE appMode = APPMODE::APPMODE_NOUI;

    DeviceList *deviceList = DeviceList::getInstance();

    for (int i = 1; i < argc; i++) {

        if (!strcmp(argv[i], "-l")) {

            LOG_S("Listing Interfaces .....");

            for (uint32_t i = 0; i < deviceList->getCount(); i++) {
                LOG_S("%s : %s", InterfaceTypes::getName(deviceList->get(i)->getType()),
                      deviceList->get(i)->getName());
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

        } else if (!strcmp(argv[i], "-w")) {

            appMode = APPMODE::APPMODE_WXWIDGETS;

            distCount = 1;
            collCount = 1;
            nodeCount = 1;

        } else if (!strcmp(argv[i], "-y")) {

            appMode = APPMODE::APPMODE_CONSOLE;

        } else if (!strcmp(argv[i], "-d")) {

            distCount = 1;

        } else if (!strcmp(argv[i], "-c")) {

            if (appMode == APPMODE::APPMODE_WXWIDGETS) {
                continue;
            }

            if (isdigit(argv[++i][0])) {
                collCount =  atoi(argv[i]);

            } else {

                return 0;
            }

        } else if (!strcmp(argv[i], "-n")) {

            if (appMode == APPMODE::APPMODE_WXWIDGETS) {
                continue;
            }

            if (isdigit(argv[++i][0])) {
                nodeCount =  atoi(argv[i]);

            } else {

                return 0;
            }

        }
    }

    LOG_S("Running in %s Mode with %d Distributor, %d Collector and %d Node, using interfaces : %s and %s",
          appMode == APPMODE::APPMODE_NOUI ? "No UI" : appMode == APPMODE::APPMODE_CONSOLE ? "Console" : "WxWidgets",
          distCount, collCount, nodeCount,
          deviceList->get(interfaceID[0])->getName(), deviceList->get(interfaceID[1])->getName());

    ComponentController *controller = ComponentController::newInstance(interfaceID[0], interfaceID[1]);

    if (distCount) {
        controller->startDistributor();
    }

    if (collCount) {
        controller->startCollector(collCount);
    }

    if (nodeCount) {
        controller->startNode(nodeCount);
    }


    if (appMode == APPMODE::APPMODE_WXWIDGETS) {
#ifdef WX_UI
        app = new WxApp(controller);

        wxApp::SetInstance((WxApp*)app);
        return wxEntry(argc, argv);
#endif
    } else if (appMode == APPMODE::APPMODE_CONSOLE) {
#ifdef CONSOLE_UI
        app = new ConsoleApp(controller);
#endif
    }

    int in;
    do {

        in = getchar();
        switch(in) {
            case 'p':
                controller->getDistributor()->sendWakeupMessagesAll(true);
                break;
            case 'l':
                controller->getCollector(0)->loadJob(NULL);
                break;
            case 'x':
                controller->getCollector(0)->processJobs();
                break;
            case 'q':
                delete controller;
#ifdef CONSOLE_UI
                if (appMode == APPMODE::APPMODE_CONSOLE) {
                    delete ((ConsoleApp*)app);
                }
#endif
                return 0;
            case 'd':
                Log::iterateLogLevel();
                break;
            default:
                break;
        }

    } while(1);
}
