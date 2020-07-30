//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#ifndef DICON_TESTAPP_H
#define DICON_TESTAPP_H

#include <DeviceList.h>
#include <Log.h>
#include <Util.h>
#include <ComponentController.h>

enum MSG_TYPE_TEST {
    MSG_TYPE_TEST_PING = 100,
    MSG_TYPE_TEST_COMPLIST,
    MSG_TYPE_TEST_PROCESS,
    MSG_TYPE_TEST_FILEINFO,
    MSG_TYPE_TEST_FILEBINARY,
    MSG_TYPE_TEST_JOBNAME,
    MSG_TYPE_TEST_WAKEUP,

};

#include "Application.h"

class TestApp : public App {

    int overrideCount[COMP_MAX] {1, 1, 1};

public:
    TestApp(int *, LOGLEVEL*, std::vector<int>&);

    void testPing(Distributor*, Collector*, Node*);
    void testComponentList(Distributor*, Collector*, Node*);
    void testProcess(Distributor*, Collector*, Node*);
    void testFileInfo(Distributor*, Collector*, Node*);
    void testFileBinary(Distributor*, Collector*, Node*);
    void testJobName(Distributor*, Collector*, Node*);
    void testLoadJob(Distributor*, Collector*, Node*);
    void testWakeUp(Distributor*, Collector*, Node*);

    int run() override;

    void help();

    int notifyHandler(COMPONENT, NOTIFYSTATE) override;
};

#endif //DICON_TESTAPP_H
