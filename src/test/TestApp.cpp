//
// Created by Haluk AKGUNDUZ on 23.07.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#include "TestApp.h"

TestApp::TestApp(AppParams& params)
        : App(APPTYPE_TEST, params) {

    MessageType::addMsg(MSG_TYPE_TEST_FILE_BINARY, "TEST_FILE_BINARY");

    registerMessageCallbacks();

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
    addRoutine('l', "ProcessList", &TestApp::testProcessList);
    addRoutine('x', "ProcessExecute", &TestApp::testProcessExecute);
    addRoutine('p', "Poll", &TestApp::poll);

    help();
}

void TestApp::registerMessageCallbacks() {

    for (auto &c : componentFactory->getCollectors()) {

        c->addStaticProcessHandler(COMP_NODE, (MSG_TYPE)MSG_TYPE_TEST_FILE_BINARY, &TestApp::processFileBinaryMsg);
    }

    for (auto &n : componentFactory->getNodes()) {

        n->addStaticProcessHandler(COMP_COLLECTOR, (MSG_TYPE)MSG_TYPE_TEST_FILE_BINARY, &TestApp::processFileBinaryMsg);
    }

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

    auto d = componentFactory->hasDistributor() ? componentFactory->getDistributor() : nullptr;
    auto c = componentFactory->getCollectorCount() ? componentFactory->getCollector(0) : nullptr;
    auto n = componentFactory->getNodeCount() ? componentFactory->getNode(0) : nullptr;

    if (d) {
        d->sendWakeupMessagesAll();
    }

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
