//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#ifndef DICON_TESTAPP_H
#define DICON_TESTAPP_H

#include <DeviceList.h>
#include <Log.h>
#include <Util.h>
#include <ComponentFactory.h>
#include "Application.h"

#define TEST_JOB_ZIP "../sample/Job1_x86_linux.zip"
#define TEST_JOB_PATH "../scratch/Job1_x86_linux"
#define TEST_JOB_FILE "matrop-multiply"

enum MSG_TYPE_TEST {
    MSG_TYPE_TEST_PING = 100,
    MSG_TYPE_TEST_COMPLIST,
    MSG_TYPE_TEST_PROCESS,
    MSG_TYPE_TEST_PROCESSID,
    MSG_TYPE_TEST_PROCESSINFO,
    MSG_TYPE_TEST_FILEINFO,
    MSG_TYPE_TEST_FILEBINARY,
    MSG_TYPE_TEST_JOBNAME,
    MSG_TYPE_TEST_WAKEUP,
    MSG_TYPE_TEST_ALIVE,
};

class TestApp : public App {

    int overrideCount[COMP_MAX] {1, 1, 1};

public:
    TestApp(int *, LOGLEVEL, std::vector<int>&);

    void testSendJobName(TypeDistributor&, TypeCollector&, TypeNode&);
    void testSendFileBinary(TypeDistributor&, TypeCollector&, TypeNode&);
    void testSendProcessID(TypeDistributor&, TypeCollector&, TypeNode&);
    void testSendProcessInfo(TypeDistributor&, TypeCollector&, TypeNode&);
    void testSendProcessFileBinary(TypeDistributor&, TypeCollector&, TypeNode&);
    void testSendProcessFiles(TypeDistributor&, TypeCollector&, TypeNode&);
    void testSendProcess(TypeDistributor&, TypeCollector&, TypeNode&);
    void testSendComponentList(TypeDistributor&, TypeCollector&, TypeNode&);

    void testPing(TypeDistributor&, TypeCollector&, TypeNode&);
    void testFileInfo(TypeDistributor&, TypeCollector&, TypeNode&);


    void testLoadJob(TypeDistributor&, TypeCollector&, TypeNode&);
    void testWakeUp(TypeDistributor&, TypeCollector&, TypeNode&);
    void testPipeControl(TypeDistributor&, TypeCollector&, TypeNode&);

    int run() override;

    void help();

    int notifyHandler(COMPONENT, NOTIFYTYPE) override;
};

#endif //DICON_TESTAPP_H
