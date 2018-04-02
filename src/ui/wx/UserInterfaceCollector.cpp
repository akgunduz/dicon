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

#include "UserInterface.h"

void UserInterface::collInit() {

    int width = collJobList->GetSize().GetWidth() / 5.5;

    collJobList->AppendColumn("ID", width * 4, wxALIGN_LEFT, 0);
    collJobList->AppendColumn("Repeat", width, wxALIGN_RIGHT, 0);

    uiUpdater[UI_UPDATE_COLL_ADDRESS] = &UserInterface::collUpdateAddresses;
    uiUpdater[UI_UPDATE_COLL_ATT_DIST_ADDRESS] = &UserInterface::collUpdateAttachedDistAddress;
    uiUpdater[UI_UPDATE_COLL_ATT_NODE_ADDRESS] = &UserInterface::collUpdateAttachedNodeAddress;
    uiUpdater[UI_UPDATE_COLL_JOB_LIST] = &UserInterface::collUpdateJobList;
    uiUpdater[UI_UPDATE_COLL_PROCESS_LIST] = &UserInterface::collUpdateProcessList;
    uiUpdater[UI_UPDATE_COLL_LOG] = &UserInterface::collUpdateLog;

}

/*
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_COLL_INIT
 */

void UserInterface::OnCollInitClick( wxCommandEvent& event )
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
        collJobList->DeleteAllItems();
    }
}


/*
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_COLL_PROCESS
 */

void UserInterface::OnCollProcessClick( wxCommandEvent& event )
{
    collProcessList->Clear();

    collObject->syncTime();

    collObject->processRule();

}

void UserInterface::OnCollJobListChecked( wxTreeListEvent& event )
{

    collJobList->UpdateItemParentStateRecursively(event.GetItem());

    wxCheckBoxState state = collJobList->GetCheckedState(event.GetItem());

    collJobList->CheckItemRecursively(event.GetItem(), state);

}

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

}

void UserInterface::collUpdateJobList(wxCommandEvent &event) {

    Jobs *jobs = (Jobs*)event.GetClientData();

    for (int j =0; j < jobs->getCount(); j++) {

        Job *job = jobs->getJob(j);
        wxTreeListItem jobItem = collJobList->AppendItem(collJobList->GetRootItem(), job->getName());
        for (int i = 0; i < job->getContentCount(CONTENT_FILE); i++) {
            Rule *rule = (Rule*) job->getContent(CONTENT_FILE, i);
            wxTreeListItem ruleItem = collJobList->AppendItem(jobItem, rule->getFileName());
            collJobList->CheckItem(ruleItem, rule->getActive() ? wxCHK_CHECKED : wxCHK_UNCHECKED);
            collJobList->SetItemText(ruleItem, 1, wxString::Format(wxT("%d"), rule->getRepeat()));
            collJobList->UpdateItemParentStateRecursively(ruleItem);
        }
    }
}

void UserInterface::collUpdateProcessList(wxCommandEvent &event) {
/*
    Rule *rule = (Rule *)event.GetClientData();

    for (int j = 0; j < rule->getContentCount(CONTENT_EXECUTOR); j++) {

        ExecutorContent *content = (ExecutorContent *) rule->getContent(CONTENT_EXECUTOR, j);
        if (content == nullptr) {
            return;
        }

        collExecList->Append(content->getExec());

    }
*/
}