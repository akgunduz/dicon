//
// Created by Haluk AKGUNDUZ on 03.05.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include <ui/wx/WxApp.h>
#include "Application.h"
#include "DeviceList.h"
#include "Util.h"

int main(int argc, char** argv) {

    Application *app;
    DeviceList *deviceList = DeviceList::getInstance();
    Util::cleanup();

    if (argc > 1) {

        char cmd = argv[1][0];

        switch(cmd) {
            case 'l':
                LOG_S("Listing Interfaces .....");
                for (uint32_t i = 0; i < deviceList->getCount(); i++) {
                    LOG_S("%s : %s", InterfaceTypes::getName(deviceList->get(i)->getType()),
                          deviceList->get(i)->getName());
                }
                break;
            case 'w':
                LOG_S("Running in WxWidgets Mode");
                {
                    if (argc == 4) {
                        app = new WxApp(atoi(argv[2]), atoi(argv[3]));
                    } else {
                        app = new WxApp();
                    }

                    wxApp::SetInstance((wxAppConsole*)app);
                    return wxEntry(argc, argv);
                }

            case 'd':
                LOG_S("Running in Console Distributor Mode");
                break;

            case 'o':
                LOG_S("Running in Console Collector Mode");
                break;

            case 'n':
                LOG_S("Running in Console Node Mode");

                if (argc != 4) {
                    LOG_E("Invalid Parameter, Must be \"Mode Interface Interface\"");
                    break;
                }

               // c_ui->nodeRun(atoi(argv[2]), atoi(argv[3]));
                break;
            default:
                break;
        }

        return 0;
    }

    return 0;
}

Application::Application(int ind1, int ind2) {

    DeviceList *deviceList = DeviceList::getInstance();
    deviceList->setActive(ind1, ind2);
}
