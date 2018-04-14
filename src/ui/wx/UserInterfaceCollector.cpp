/////////////////////////////////////////////////////////////////////////////
// Name:        UserInterface.cpp
// Purpose:     
// Author:      Haluk Akgunduz
// Modified by: 
// Created:     Thu 22 Oct 2015 14:22:19 EEST
// RCS-ID:      
// Copyright:   All rights reserved
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#include "ExecutorItem.h"
#include "UserInterface.h"

void UserInterface::collInit() {

//    int width = collJobList->GetSize().GetWidth() / 5.5;
//
//    collJobList->AppendColumn("ID", width * 4, wxALIGN_LEFT, 0);
//    collJobList->AppendColumn("Repeat", width, wxALIGN_RIGHT, 0);

    uiUpdater[UI_UPDATE_COLL_ADDRESS] = &UserInterface::collUpdateAddresses;
    uiUpdater[UI_UPDATE_COLL_ATT_DIST_ADDRESS] = &UserInterface::collUpdateAttachedDistAddress;
    uiUpdater[UI_UPDATE_COLL_ATT_NODE_ADDRESS] = &UserInterface::collUpdateAttachedNodeAddress;
    uiUpdater[UI_UPDATE_COLL_FILE_LIST] = &UserInterface::collUpdateFileList;
    uiUpdater[UI_UPDATE_COLL_PROCESS_LIST] = &UserInterface::collUpdateProcessList;
    uiUpdater[UI_UPDATE_COLL_LOG] = &UserInterface::collUpdateLog;

}

/*
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_COLL_INIT
 */

void UserInterface::OnCollInitClickWrapper( wxCommandEvent& event )
{
    if (wxStrcmp(collInitBtn->GetLabel(), "Init") == 0) {

        try {
            char path[PATH_MAX];
            sprintf(path, "%s/%s", getcwd(nullptr, 0), COLLECTOR_PATH);
            mkdir(path, 0777);

            collObject = new Collector(path);

        } catch (std::runtime_error &e) {

            return;
        }

        collLog->Clear();
        collProcessBtn->Enable(true);
        collInitBtn->SetLabel("Stop");

    } else {

        delete collObject;
        collProcessBtn->Enable(false);
        collInitBtn->SetLabel("Init");
        collDistDeviceAddress->SetLabel("");
        collNodeDeviceAddress->SetLabel("");
        collFileList->Clear();
        collProcessList->Clear();
      //  collJobList->DeleteAllItems();
    }
}


/*
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_COLL_PROCESS
 */

void UserInterface::OnCollProcessClickWrapper( wxCommandEvent& event )
{
    collObject->processJob();
}

//void UserInterface::OnCollJobListChecked( wxTreeListEvent& event )
//{
//
//    collJobList->UpdateItemParentStateRecursively(event.GetItem());
//
//    wxCheckBoxState state = collJobList->GetCheckedState(event.GetItem());
//
//    collJobList->CheckItemRecursively(event.GetItem(), state);
//
//}

void UserInterface::collUpdateAddresses(wxCommandEvent &event) {

    EventData *data = (EventData *)event.GetClientData();
    collDistDeviceAddress->SetLabel(InterfaceTypes::getAddressString(data->data64_1));
    collNodeDeviceAddress->SetLabel(InterfaceTypes::getAddressString(data->data64_2));

}

void UserInterface::collUpdateAttachedDistAddress(wxCommandEvent &event) {

    EventData *data = (EventData *)event.GetClientData();
    collDistAddress->SetLabel(InterfaceTypes::getAddressString(data->data64_1));

}

void UserInterface::collUpdateAttachedNodeAddress(wxCommandEvent &event) {

    EventData *data = (EventData *)event.GetClientData();
    collNodeAddress->SetLabel(InterfaceTypes::getAddressString(data->data64_1));

}

void UserInterface::collUpdateLog(wxCommandEvent &event) {

    EventData *data = (EventData *)event.GetClientData();

    collLog->Append(wxString::Format("%s", data->dataStr));
    LOG_S("%s : %s", ComponentTypes::getName(COMP_COLLECTOR), data->dataStr.c_str());
}

void UserInterface::collUpdateFileList(wxCommandEvent &event) {

    Job *job = (Job*)event.GetClientData();

    for (int j = 0; j < job->getContentCount(CONTENT_FILE); j++) {

        FileItem *content = (FileItem *) job->getContent(CONTENT_FILE, j);
        if (content == nullptr) {
            return;
        }

        collFileList->Append(content->getFileName());

    }
}

void UserInterface::collUpdateProcessList(wxCommandEvent &event) {

    Job *job = (Job *)event.GetClientData();

    for (int j = 0; j < job->getContentCount(CONTENT_EXECUTOR); j++) {

        ExecutorItem *content = (ExecutorItem *) job->getContent(CONTENT_EXECUTOR, j);
        if (content == nullptr) {
            return;
        }

        collProcessList->Append(content->getExec());

    }

}