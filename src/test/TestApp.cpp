//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include "TestApp.h"

TestApp::TestApp(int *_interfaces, LOGLEVEL _logLevel, std::vector<int>& _componentCount)
        : App(APPTYPE_TEST, _interfaces, _logLevel, {1, 1, 1}, false) {

    help();
}

void TestApp::help() {

    LOGP_I("Commands : ");
    LOGP_I("\tTest Ping           : 'p'");
    LOGP_I("\tTest Component List : 'c'");
    LOGP_I("\tTest Process        : 's'");
    LOGP_I("\tTest File Info      : 'i'");
    LOGP_I("\tTest File Binary    : 'b'");
    LOGP_I("\tTest Job Name       : 'j'");
    LOGP_I("\tTest Load Job       : 'l'");
    LOGP_I("\tTest WakeUp         : 'w'");
    LOGP_I("\tTest Pipe           : 'e'");
    LOGP_I("\tHelp                : 'h'");
    LOGP_I("\tQuit                : 'q'");
}

int TestApp::run() {

    auto &d = componentController->getDistributor();
    auto &c = componentController->getCollector(0);
    auto &n = componentController->getNode(0);

    int in;
    do {

        in = getchar();
        switch(in) {

            case 'j':
                testSendJobName(d, c, n);
                break;
            case 'b':
                testSendFileBinary(d, c, n);
                break;
            case 'y':
                testSendProcessID(d, c, n);
                break;
            case 's':
                testSendProcessInfo(d, c, n);
                break;
            case 't':
                testSendProcessFileBinary(d, c, n);
                break;
            case 'a':
                testSendProcessFiles(d, c, n);
                break;
            case 'f':
                testSendProcess(d, c, n);
                break;
            case 'c':
                testSendComponentList(d, c, n);
                break;



            case 'p':
                testPing(d, c, n);
                break;
            case 'i':
                testFileInfo(d, c, n);
                break;
            case 'l':
                testLoadJob(d, c, n);
                break;
            case 'w':
                testWakeUp(d, c, n);
                break;
            case 'e':
                testPipeControl(d, c, n);
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


int TestApp::notifyHandler(COMPONENT target, NOTIFYTYPE id) {

    return 0;
}
