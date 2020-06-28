//
// Created by Haluk AKGUNDUZ on 27.06.2020.
// Copyright (c) 2020 Haluk Akgunduz. All rights reserved.
//

#include <ui/UserInterfaceEvent.h>
#include <Job.h>
#include "WebApp.h"

void WebApp::collInit() {

    uiUpdater[UI_UPDATE_COLL_ID] = &WebApp::collUpdateID;
    uiUpdater[UI_UPDATE_COLL_FILE_LIST] = &WebApp::collUpdateFileList;
    uiUpdater[UI_UPDATE_COLL_FILE_LISTITEM] = &WebApp::collUpdateFileListItem;
    uiUpdater[UI_UPDATE_COLL_PROCESS_LIST] = &WebApp::collUpdateProcessList;
    uiUpdater[UI_UPDATE_COLL_PROCESS_LISTITEM] = &WebApp::collUpdateProcessListItem;

}

void WebApp::OnCollLoadClick(WebEvent &event)
{
//    collFileList->DeleteAllItems();
//    collProcessList->DeleteAllItems();

    ((ComponentController *)componentController)->getCollector(0)->loadJob(NULL);
}

void WebApp::OnCollProcessClick(WebEvent &event)
{
    ((ComponentController *)componentController)->getCollector(0)->processJobs();
}

void WebApp::collUpdateID(WebEvent &event) {

}

void WebApp::collUpdateFileList(WebEvent &event) {

    auto *data = (UserInterfaceEvent *)event.GetClientData();

    auto *job = (Job*)data->getPointer(0);

//    for (int j = 0; j < job->getFileCount(); j++) {
//
//        auto *content = job->getFile(j);
//
//        LOG_S("Console UI ------> Collector File Added : %s, Validity : %s",
//              content->getFileName(), content->isValid() ? "Valid" : "Invalid");
//    }
}

void WebApp::collUpdateFileListItem(WebEvent &event) {

    auto *data = (UserInterfaceEvent *)event.GetClientData();

//    for (int i = 0; i < data->getDataSize(); i++) {
//
//        LOG_S("Console UI ------> Collector File Validated with index : %d", data->getData(i));
//    }
}

void WebApp::collUpdateProcessList(WebEvent &event) {

    auto *data = (UserInterfaceEvent *)event.GetClientData();

    auto *job = (Job *)data->getPointer(0);

//    for (int j = 0; j < job->getOrderedCount(); j++) {
//
//        LOG_S("Console UI ------> Collector Process[%d] : %s is triggered", j, job->getOrderedExecution(j)->getExec());
//
//    }
}

void WebApp::collUpdateProcessListItem(WebEvent &event) {

    auto *data = (UserInterfaceEvent *)event.GetClientData();

    auto *job = (Job *)data->getPointer(0);

    for (int j = 0; j < job->getOrderedCount(); j++) {

//        switch(job->getOrderedState(j)) {
//
//            case PROCESS_STATE_ENDED:
//                LOG_S("Console UI ------> Collector Process[%d] is ended", j);
//                break;
//
//            case PROCESS_STATE_STARTED:
//                LOG_S("Console UI ------> Collector Process[%d] is started", j);
//                break;
//
//            case PROCESS_STATE_NOTSTARTED:
//                LOG_S("Console UI ------> Collector Process[%d] is not started", j);
//                break;
//
//            default:
//                break;
//        }
    }
}
