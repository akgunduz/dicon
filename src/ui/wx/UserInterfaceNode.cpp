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

void UserInterface::nodeInit() {

    int width = nodeFileList->GetSize().GetWidth() / 5 - 1;

    wxListItem column;
    column.SetId(0);
    column.SetText( _("Name") );
    column.SetWidth(width * 4);
    nodeFileList->InsertColumn(0, column);

    column.SetId(1);
    column.SetText( _("State") );
    column.SetWidth(width);
    nodeFileList->InsertColumn(1, column);

    uiUpdater[UI_UPDATE_NODE_ADDRESS] = &UserInterface::nodeUpdateAddresses;
    uiUpdater[UI_UPDATE_NODE_STATE] = &UserInterface::nodeUpdateState;
    uiUpdater[UI_UPDATE_NODE_ATT_COLL_ADDRESS] = &UserInterface::nodeUpdateAttachedCollAddress;
    uiUpdater[UI_UPDATE_NODE_FILE_LIST] = &UserInterface::nodeUpdateFileList;
    uiUpdater[UI_UPDATE_NODE_EXEC_LIST] = &UserInterface::nodeUpdateExecList;
    uiUpdater[UI_UPDATE_NODE_CLEAR] = &UserInterface::nodeUpdateClear;
    uiUpdater[UI_UPDATE_NODE_LOG] = &UserInterface::nodeUpdateLog;
/*
    if (Device::getCount() > 0) {
        for (uint32_t i = 0; i < Device::getCount(); i++) {
            nodeConnectInterface->Insert(wxString(sInterfaces[Device::getType(i)]) + " --> " + Device::getName(i), i);
        }
        nodeConnectInterface->Select(0);
    }
*/
}

/*
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_NODE_INIT
 */

void UserInterface::OnNodeInitClick( wxCommandEvent& event )
{
    if (wxStrcmp(nodeInitBtn->GetLabel(), "Init") == 0) {

        try {
            char path[PATH_MAX];
            sprintf(path, "%s/%s", getcwd(nullptr, 0), NODE_PATH);
            mkdir(path, 0777);

            Connector::setSelectedDevices((unsigned char)distCollInterface->GetSelection(),
                                          (unsigned char)nodeInterface->GetSelection());

            nodeObject = new Node(path);

        } catch (std::runtime_error &e) {

            return;
        }

        nodeLog->Clear();
        nodeInitBtn->SetLabel("Stop");

    } else {

        delete nodeObject;
        nodeInitBtn->SetLabel("Init");
        nodeDeviceAddress->SetLabel("");
    }
}

void UserInterface::nodeUpdateAddresses(wxCommandEvent &event) {

    EventData *data = (EventData *)event.GetClientData();
    nodeDeviceAddress->SetLabel(Address::getString(data->data64_1));
}

void UserInterface::nodeUpdateState(wxCommandEvent &event) {

    EventData *data = (EventData *)event.GetClientData();
    nodeState->SetLabel(sStates[data->data64_1]);
}

void UserInterface::nodeUpdateAttachedCollAddress(wxCommandEvent &event) {

    EventData *data = (EventData *)event.GetClientData();
    nodeCollAddress->SetLabel(Address::getString(data->data64_1));
}

void UserInterface::nodeUpdateClear(wxCommandEvent &event) {

    nodeFileList->DeleteAllItems();
    nodeExecList->Clear();
}

void UserInterface::nodeUpdateLog(wxCommandEvent &event) {

    EventData *data = (EventData *)event.GetClientData();
    nodeLog->Append(wxString::Format("%s", data->dataStr));

}

void UserInterface::nodeUpdateFileList(wxCommandEvent &event) {

    Rule *rule = (Rule *)event.GetClientData();

    for (int j = 0; j < rule->getContentCount(CONTENT_FILE); j++) {

        FileItem *content = (FileItem *) rule->getContent(CONTENT_FILE, j);
        if (content == nullptr) {
            return;
        }

        long i = 0;

        for (; i < nodeFileList->GetItemCount(); i++) {

            if (nodeFileList->GetItemText(i, 0).Cmp(content->getFileName()) == 0) {
                break;
            }

        }

        if (i == nodeFileList->GetItemCount()) {
            i = nodeFileList->InsertItem(nodeFileList->GetItemCount(), "");
        }

        nodeFileList->SetItem(i, 0, content->getFileName());
        nodeFileList->SetItem(i, 1, content->isValid() ? "V" : "I");
    }

}

void UserInterface::nodeUpdateExecList(wxCommandEvent &event) {

    Rule *rule = (Rule *)event.GetClientData();

    for (int j = 0; j < rule->getContentCount(CONTENT_EXECUTOR); j++) {

        ExecutorItem *content = (ExecutorItem *) rule->getContent(CONTENT_EXECUTOR, j);
        if (content == nullptr) {
            return;
        }

        nodeExecList->Append(content->getExec());

    }

}
