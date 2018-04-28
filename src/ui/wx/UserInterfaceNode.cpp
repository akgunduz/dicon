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

}

/*
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_NODE_INIT
 */

void UserInterface::OnNodeInitClickWrapper( wxCommandEvent& event )
{
    if (wxStrcmp(nodeInitBtn->GetLabel(), "Init") == 0) {

        try {

            nodeObject = Node::newInstance(1);

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
    nodeDeviceAddress->SetLabel(InterfaceTypes::getAddressString(data->data64_1));
}

void UserInterface::nodeUpdateState(wxCommandEvent &event) {

    EventData *data = (EventData *)event.GetClientData();
    nodeState->SetLabel(NodeState::getName((NODE_STATES)data->data64_1));
}

void UserInterface::nodeUpdateAttachedCollAddress(wxCommandEvent &event) {

    EventData *data = (EventData *)event.GetClientData();
    nodeCollAddress->SetLabel(InterfaceTypes::getAddressString(data->data64_1));
}

void UserInterface::nodeUpdateClear(wxCommandEvent &event) {

    nodeFileList->DeleteAllItems();
    nodeExecList->Clear();
}

void UserInterface::nodeUpdateLog(wxCommandEvent &event) {

    EventData *data = (EventData *)event.GetClientData();

    nodeLog->Append(wxString::Format("%s", data->dataStr));
    LOG_S("%s", data->dataStr.c_str());
}

void UserInterface::nodeUpdateFileList(wxCommandEvent &event) {

    Job *job = (Job *)event.GetClientData();

    for (int j = 0; j < job->getContentCount(CONTENT_FILE); j++) {

        FileItem *content = (FileItem *) job->getContent(CONTENT_FILE, j);
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

    EventData *data = (EventData *)event.GetClientData();
    nodeExecList->Append(wxString::Format("%s", data->dataStr));
}
