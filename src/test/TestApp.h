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

enum MSG_TYPE_TEST {

    MSG_TYPE_TEST_JOBNAME = 0x100,
    MSG_TYPE_TEST_COMPONENT,
    MSG_TYPE_TEST_COMPONENTS,
    MSG_TYPE_TEST_FILE_INFO,
    MSG_TYPE_TEST_FILE_BINARY,
    MSG_TYPE_TEST_PROCESS,
    MSG_TYPE_TEST_PROCESSES,
    MSG_TYPE_TEST_PROCESS_ID,
    MSG_TYPE_TEST_PROCESS_INFO,
    MSG_TYPE_TEST_PROCESS_FILE_INFO,
    MSG_TYPE_TEST_PROCESS_FILE_BINARY,
    MSG_TYPE_TEST_PROCESS_FILES_INFO,
    MSG_TYPE_TEST_PROCESS_FILES_BINARY,

    MSG_TYPE_TEST_WAKEUP,
    MSG_TYPE_TEST_ID,
    MSG_TYPE_TEST_ALIVE,
    MSG_TYPE_TEST_PING,
};

class TestApp;

typedef void (TestApp::*TypeTestRoutine)(TypeDistributor&, TypeCollector&, TypeNode&);
typedef std::pair<std::string, TypeTestRoutine> TypeTest;
typedef std::map<char, TypeTest> TypeTestList;

class TestApp : public App {

    int overrideCount[COMP_MAX] {1, 1, 1};
    TypeTestList list;

    inline void addRoutine(char id, const std::string& name, TypeTestRoutine routine) {

        list[id] = std::make_pair(name, routine);
    }

    inline bool callRoutine(char id, TypeDistributor& d, TypeCollector& c, TypeNode& n) {

        auto test = list.find(id);
        if (test == list.end()) {

            return false;
        }

        (this->*test->second.second)(d, c, n);

        return true;
    }

public:
    TestApp(int *, const LogInfo&, std::vector<int>&);

    void testSendJobName(TypeDistributor&, TypeCollector&, TypeNode&);
    void testSendFileBinary(TypeDistributor&, TypeCollector&, TypeNode&);
    void testSendFileInfo(TypeDistributor&, TypeCollector&, TypeNode&);
    void testSendProcessID(TypeDistributor&, TypeCollector&, TypeNode&);
    void testSendProcessInfo(TypeDistributor&, TypeCollector&, TypeNode&);
    void testSendProcessFileBinary(TypeDistributor&, TypeCollector&, TypeNode&);
    void testSendProcessFilesBinary(TypeDistributor&, TypeCollector&, TypeNode&);
    void testSendProcess(TypeDistributor&, TypeCollector&, TypeNode&);
    void testSendComponentList(TypeDistributor&, TypeCollector&, TypeNode&);
    void testSendWakeUp(TypeDistributor&, TypeCollector&, TypeNode&);
    void testSendID(TypeDistributor&, TypeCollector&, TypeNode&);

    void testPing(TypeDistributor&, TypeCollector&, TypeNode&);


    void testLoadJob(TypeDistributor&, TypeCollector&, TypeNode&);
    void testCRC(TypeDistributor&, TypeCollector&, TypeNode&);

    void testProcessExecute(TypeDistributor&, TypeCollector&, TypeNode&);
    void testFileExecute(TypeDistributor&, TypeCollector&, TypeNode&);

    int run() override;

    void help();

    int notifyHandler(COMPONENT, NOTIFYTYPE) override;
};

#endif //DICON_TESTAPP_H
