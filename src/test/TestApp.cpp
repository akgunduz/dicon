//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include "TestApp.h"

TestApp::TestApp(int *_interfaces, LOGLEVEL* _logLevel, std::vector<int>& _componentCount)
        : App(APPTYPE_TEST, _interfaces, _logLevel, {1, 1, 1}) {

    help();
}

void TestApp::help() {

    PRINT("Commands : ");
    PRINT("\tTest Ping           : 'p'");
    PRINT("\tTest Component List : 'c'");
    PRINT("\tTest Process        : 's'");
    PRINT("\tTest File Info      : 'i'");
    PRINT("\tTest File Binary    : 'b'");
    PRINT("\tTest Job Name       : 'x'");
    PRINT("\tTest Load Job       : 'l'");
    PRINT("\tTest WakeUp         : 'w'");
    PRINT("\tHelp                : 'h'");
    PRINT("\tQuit                : 'q'");
}

int TestApp::run() {

    Distributor *d = componentController->getDistributor();
    Collector *c = componentController->getCollector(0);
    Node *n = componentController->getNode(0);

    int in;
    do {

        in = getchar();
        switch(in) {
            case 'p':
                testPing(d, c, n);
                break;
            case 'c':
                testComponentList(d, c, n);
                break;
            case 's':
                testProcess(d, c, n);
                break;
            case 'i':
                testFileInfo(d, c, n);
                break;
            case 'b':
                testFileBinary(d, c, n);
                break;
            case 'j':
                testJobName(d, c, n);
                break;
            case 'l':
                testLoadJob(d, c, n);
                break;
            case 'w':
                testWakeUp(d, c, n);
                break;
            case 'h':
                help();
                break;
            case 'q':
                return 0;
            default:
                break;
        }

    } while(true);
}


int TestApp::notifyHandler(COMPONENT target, NOTIFYSTATE id) {

    return 0;
}
