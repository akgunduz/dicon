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

#include "WxComponent.h"

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

        nodeInitBtn->SetLabel("Stop");

    } else {

        delete ((Node*)nodeObject);
        nodeInitBtn->SetLabel("Init");
        nodeDeviceAddress->SetLabel("");
    }
}

void UserInterface::nodeUpdateAddresses(wxCommandEvent &event) {

    auto *data = (UserInterfaceEvent *)event.GetClientData();

    nodeDeviceAddress->SetLabel(InterfaceTypes::getAddressString(data->getData(0)));
}

void UserInterface::nodeUpdateState(wxCommandEvent &event) {

    auto *data = (UserInterfaceEvent *)event.GetClientData();

    nodeState->SetLabel(NodeState::getName((NODE_STATES)data->getData(0)));
}

void UserInterface::nodeUpdateAttachedCollAddress(wxCommandEvent &event) {

    auto *data = (UserInterfaceEvent *)event.GetClientData();

    nodeCollAddress->SetLabel(InterfaceTypes::getAddressString(data->getData(0)));
}

void UserInterface::nodeUpdateClear(wxCommandEvent &event) {

    nodeFileList->DeleteAllItems();
    nodeExecList->Clear();
}

void UserInterface::nodeUpdateFileList(wxCommandEvent &event) {

    auto *data = (UserInterfaceEvent *)event.GetClientData();

    auto *job = (Job *)data->getPointer(0);

    for (int j = 0; j < job->getContentCount(CONTENT_FILE); j++) {

        auto *content = (FileItem *) job->getContent(CONTENT_FILE, j);
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

    auto *data = (UserInterfaceEvent *)event.GetClientData();

    nodeExecList->Append(wxString::Format("%s", data->getString(0)));
}
