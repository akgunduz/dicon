//
// Created by Haluk AKGUNDUZ on 03.05.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include <ui/wx/WxApp.h>
#include "Application.h"
#include "DeviceList.h"
#include "Util.h"
#include "ComponentController.h"

Application::Application(int ind1, int ind2, bool initState) {

    setStartState(initState);
    DeviceList *deviceList = DeviceList::getInstance();
    deviceList->setActive(ind1, ind2);
}

void Application::setStartState(bool state) {

    this->initStart = state;
}

bool Application::getStartState() {

    return initStart;
}

int main(int argc, char** argv) {

    Log::init(LEVEL_INFO);

    Util::cleanup();

    for (int i = 1; i < argc; i++) {

        if (!strcmp(argv[i], "-l")) {

            LOG_S("Listing Interfaces .....");
            DeviceList *deviceList = DeviceList::getInstance();
            for (uint32_t i = 0; i < deviceList->getCount(); i++) {
                LOG_S("%s : %s", InterfaceTypes::getName(deviceList->get(i)->getType()),
                      deviceList->get(i)->getName());
            }

            return 0;

        } else if (!strcmp(argv[i], "-w")) {

            LOG_S("Running in WxWidgets Mode");

            int interfaceID[2] = {0, 0};

            if (argc > i + 1) {

                if (isdigit(argv[++i][0])) {
                    interfaceID[0] = atoi(argv[i]);

                } else {
                    break;
                }

                if (isdigit(argv[++i][0])) {
                    interfaceID[1] = atoi(argv[i]);

                } else {
                    break;
                }
            }

            ComponentController *controller = ComponentController::newInstance(interfaceID[0], interfaceID[1]);
            wxApp *app = new WxApp(controller);

            wxApp::SetInstance(app);
            return wxEntry(argc, argv);

        } else if (!strcmp(argv[i], "-d")) {

            LOG_S("Listing Interfaces .....");
            DeviceList *deviceList = DeviceList::getInstance();
            for (uint32_t i = 0; i < deviceList->getCount(); i++) {
                LOG_S("%s : %s", InterfaceTypes::getName(deviceList->get(i)->getType()),
                      deviceList->get(i)->getName());
            }

            return 0;

        }

    }

    return 0;
}
