//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include "TestApp.h"

TestApp::TestApp(int *_interfaces, const LogInfo& _logInfo, std::vector<int>& _componentCount)
        : App(APPTYPE_TEST, _interfaces, _logInfo, {1, 1, 1}, false) {

    addRoutine('1', "SendWakeUp", &TestApp::testSendWakeUp);
    addRoutine('2', "SendID", &TestApp::testSendID);
    addRoutine('3', "SendJobName", &TestApp::testSendJobName);
    addRoutine('4', "SendFileBinary", &TestApp::testSendFileBinary);
    addRoutine('5', "SendProcessID", &TestApp::testSendProcessID);
    addRoutine('6', "SendProcessInfo", &TestApp::testSendProcessInfo);
    addRoutine('7', "SendProcessFileBinary", &TestApp::testSendProcessFileBinary);
    addRoutine('8', "SendProcessFilesBinary", &TestApp::testSendProcessFilesBinary);
    addRoutine('9', "SendProcess", &TestApp::testSendProcess);
    addRoutine('a', "SendComponentList", &TestApp::testSendComponentList);
    addRoutine('b', "CRC", &TestApp::testCRC);
    addRoutine('c', "Ping", &TestApp::testPing);
    addRoutine('d', "LoadJob", &TestApp::testLoadJob);
    addRoutine('x', "ProcessExecute", &TestApp::testProcessExecute);
    addRoutine('y', "FileExecute", &TestApp::testFileExecute);

    help();
}

void TestApp::help() {

    LOGP_I("Commands : ");
    for (const auto& test : list) {

        LOGP_I("\tTest %25s: '%c'", test.second.first.c_str(), test.first);
    }

    LOGP_I("\tHelp                          : 'h'");
    LOGP_I("\tQuit                          : 'q'");
}

int TestApp::run() {

    auto &d = componentFactory->getDistributor();
    auto &c = componentFactory->getCollector(0);
    auto &n = componentFactory->getNode(0);

    testFileExecute(d, c, n);

    int in;
    do {

        in = getchar();
        switch(in) {
            case 'h':
                help();
                break;
            case 'q':
                return 0;
            default:
                callRoutine((char)in, d, c, n);
                break;
        }

    } while(true);
}


int TestApp::notifyHandler(COMPONENT target, NOTIFYTYPE id) {

    return 0;
}
