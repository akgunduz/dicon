//
// Created by Haluk Akgunduz on 02/09/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include <ui/UserInterfaceEvent.h>
#include <Job.h>
#include "ConsoleApp.h"

void ConsoleApp::collInit() {

    uiUpdater[UI_UPDATE_COLL] = &ConsoleApp::collUpdate;

}

void ConsoleApp::collUpdate(ConsoleEvent &event) {

}
//
//void ConsoleApp::collUpdateFileList(ConsoleEvent &event) {
//
//    auto *data = (UserInterfaceEvent *)event.GetClientData();
//
//    auto *job = (Job*)data->getPointer(0);
//
//    for (int j = 0; j < job->getFileCount(); j++) {
//
//        auto *content = job->getFile(j);
//
//        LOG_S("Console UI ------> Collector File Added : %s, Validity : %s",
//              content->getFileName(), content->isValid() ? "Valid" : "Invalid");
//    }
//}
//
//void ConsoleApp::collUpdateFileListItem(ConsoleEvent &event) {
//
//    auto *data = (UserInterfaceEvent *)event.GetClientData();
//
//    for (int i = 0; i < data->getDataSize(); i++) {
//
//        LOG_S("Console UI ------> Collector File Validated with index : %d", data->getData(i));
//    }
//}
//
//void ConsoleApp::collUpdateProcessList(ConsoleEvent &event) {
//
//    auto *data = (UserInterfaceEvent *)event.GetClientData();
//
//    auto *job = (Job *)data->getPointer(0);
//
////    for (int j = 0; j < job->getOrderedCount(); j++) {
////
////        LOG_S("Console UI ------> Collector Process[%d] : %s is going to triggered",
////                j, job->getOrderedExecution(j)->getExec());
////
////    }
//}
//
//void ConsoleApp::collUpdateProcessListItem(ConsoleEvent &event) {
//
//    auto *data = (UserInterfaceEvent *)event.GetClientData();
//
//    auto *job = (Job *)data->getPointer(0);
//
////    for (int j = 0; j < job->getOrderedCount(); j++) {
////
////        switch(job->getOrderedState(j)) {
////
////            case PROCESS_STATE_ENDED:
////                LOG_S("Console UI ------> Collector Process[%d] is ended", j);
////                break;
////
////            case PROCESS_STATE_STARTED:
////                LOG_S("Console UI ------> Collector Process[%d] is started", j);
////                break;
////
////            case PROCESS_STATE_DEPENDENT:
////                LOG_S("Console UI ------> Collector Process[%d] is not started", j);
////                break;
////
////            default:
////                break;
////        }
////    }
//}
