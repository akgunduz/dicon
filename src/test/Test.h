//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#ifndef DICON_TEST_H
#define DICON_TEST_H

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

};

void testPing(Distributor* distributor, Collector* collector, Node* node);
void testComponentList(Distributor* distributor, Collector* collector, Node* node);
void testProcess(Distributor* distributor, Collector* collector, Node* node);
void testFileInfo(Distributor* distributor, Collector* collector, Node* node);
void testFileBinary(Distributor* distributor, Collector* collector, Node* node);
void testJobName(Distributor* distributor, Collector* collector, Node* node);

#endif //DICON_TEST_H
