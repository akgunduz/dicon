//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#ifndef DICON_TESTAPP_H
#define DICON_TESTAPP_H

#include <DeviceList.h>
#include <Log.h>
#include <Util.h>
#include <ComponentFactory.h>

enum MSG_TYPE_TEST {
    MSG_TYPE_TEST_PING = 100,
    MSG_TYPE_TEST_COMPLIST,
    MSG_TYPE_TEST_PROCESS,
    MSG_TYPE_TEST_FILEINFO,
    MSG_TYPE_TEST_FILEBINARY,
    MSG_TYPE_TEST_JOBNAME,
    MSG_TYPE_TEST_WAKEUP,
    MSG_TYPE_TEST_ALIVE,
};

#include "Application.h"

class TestApp : public App {

    int overrideCount[COMP_MAX] {1, 1, 1};

public:
    TestApp(int *, LOGLEVEL, std::vector<int>&);

    void testPing(TypeDistributor&, TypeCollector&, TypeNode&);
    void testComponentList(TypeDistributor&, TypeCollector&, TypeNode&);
    void testProcess(TypeDistributor&, TypeCollector&, TypeNode&);
    void testFileInfo(TypeDistributor&, TypeCollector&, TypeNode&);
    void testFileBinary(TypeDistributor&, TypeCollector&, TypeNode&);
    void testJobName(TypeDistributor&, TypeCollector&, TypeNode&);
    void testLoadJob(TypeDistributor&, TypeCollector&, TypeNode&);
    void testWakeUp(TypeDistributor&, TypeCollector&, TypeNode&);
    void testPipeControl(TypeDistributor&, TypeCollector&, TypeNode&);

    int run() override;

    void help();

    int notifyHandler(COMPONENT, NOTIFYSTATE) override;
};

#endif //DICON_TESTAPP_H
