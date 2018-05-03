//
// Created by Haluk AKGUNDUZ on 03.05.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include <ui/wx/WxApp.h>
#include "Application.h"
#include "DeviceList.h"
#include "Util.h"

int main(int argc, char** argv) {

    Util::cleanup();

    if (argc > 1) {

        char cmd = argv[1][0];

        switch(cmd) {
            case 'l': {
                LOG_S("Listing Interfaces .....");
                DeviceList *deviceList = DeviceList::getInstance();
                for (uint32_t i = 0; i < deviceList->getCount(); i++) {
                    LOG_S("%s : %s", InterfaceTypes::getName(deviceList->get(i)->getType()),
                          deviceList->get(i)->getName());
                }
            }
                break;
            case 'w':
                LOG_S("Running in WxWidgets Mode");
                {
                    wxApp *app;
                    if (argc == 4) {
                        app = new WxApp(new Application(atoi(argv[2]), atoi(argv[3])));
                    } else {
                        app = new WxApp(new Application(0, 0, false));
                    }

                    wxApp::SetInstance(app);
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

Application::Application(int ind1, int ind2, bool initState) {

    setStartState(initState);
    DeviceList *deviceList = DeviceList::getInstance();
    deviceList->setActive(ind1, ind2);
}

Distributor *Application::getDistributor() {

    return distributor;
}

Collector *Application::getCollector(int index) {

    return collectors[index];
}

Node *Application::getNode(int index) {

    return nodes[index];
}

bool Application::startDistributor() {

    char path[PATH_MAX];
    sprintf(path, "%s/%s", getcwd(NULL, 0), DISTRIBUTOR_PATH);
    mkdir(path, 0777);

    distributor = Distributor::newInstance(path);

    return true;
}

bool Application::startCollector(int count) {

    for (int i = 0; i < count; i++) {

        char path[PATH_MAX];
        sprintf(path, "%s/%s_%ld", getcwd(NULL, 0), COLLECTOR_PATH, collectors.size() + 1);
        mkdir(path, 0777);

        collectors.push_back(Collector::newInstance(path));
    }

    return true;
}

bool Application::startNode(int count) {

    for (int i = 0; i < count; i++) {

        char path[PATH_MAX];
        sprintf(path, "%s/%s_%ld", getcwd(NULL, 0), NODE_PATH, nodes.size() + 1);
        mkdir(path, 0777);

        nodes.push_back(Node::newInstance(path));
    }

    return true;
}

void Application::setStartState(bool state) {

    this->initStart = state;
}

bool Application::getStartState() {

    return initStart;
}
