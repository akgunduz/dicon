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

void UserInterface::distInit() {

    int width = distCollList->GetSize().GetWidth() / 2 - 1;

    wxListItem column;
    column.SetId(0);
    column.SetText( _("Address") );
    column.SetWidth(width);
    distCollList->InsertColumn(0, column);

    column.SetId(1);
    column.SetText( _("Attached Node") );
    column.SetWidth(width);
    distCollList->InsertColumn(1, column);

    width = distNodeList->GetSize().GetWidth() / 2 - 1;

    column.SetId(0);
    column.SetText( _("Address") );
    column.SetWidth(width);
    distNodeList->InsertColumn(0, column);

    column.SetId(1);
    column.SetText( _("State") );
    column.SetWidth(width);
    distNodeList->InsertColumn(1, column);

    uiUpdater[UI_UPDATE_DIST_ADDRESS] = &UserInterface::distUpdateAddresses;
    uiUpdater[UI_UPDATE_DIST_COLL_LIST] = &UserInterface::distAddtoCollectorList;
    uiUpdater[UI_UPDATE_DIST_NODE_LIST] = &UserInterface::distAddtoNodeList;
    uiUpdater[UI_UPDATE_DIST_BACKUP] = &UserInterface::distUpdateBackup;
    uiUpdater[UI_UPDATE_DIST_LOG] = &UserInterface::distUpdateLog;
}

/*
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_DIST_INIT
 */

void UserInterface::OnDistInitClickWrapper( wxCommandEvent& event )
{
    if (wxStrcmp(distInitBtn->GetLabel(), "Init") == 0) {

        try {
            distCollList->DeleteAllItems();
            distNodeList->DeleteAllItems();

            char path[PATH_MAX];
            sprintf(path, "%s/%s", getcwd(nullptr, 0), DISTRIBUTOR_PATH);
            mkdir(path, 0777);

            double backupRate = 0;
            distBackupRate->GetLineText(0).ToDouble(&backupRate);

            distObject = new Distributor(path, backupRate);

        } catch (std::runtime_error &e) {

            return;
        }

        distLog->Clear();
        distBackupRate->SetEditable(false);
        distPollBtn->Enable(true);
        distInitBtn->SetLabel("Stop");

    } else {

        delete distObject;
        distBackupRate->SetEditable(true);
        distPollBtn->Enable(false);
        distInitBtn->SetLabel("Init");
        distCollDeviceAddress->SetLabel("");
        distNodeDeviceAddress->SetLabel("");
    }
}

void UserInterface::OnDistPollClickWrapper( wxCommandEvent& event )
{
    distObject->reset();
    distCollList->DeleteAllItems();
    distNodeList->DeleteAllItems();

    distObject->sendWakeupMessagesAll();
}

void UserInterface::distUpdateAddresses(wxCommandEvent &event) {

    EventData *data = (EventData *)event.GetClientData();
    distCollDeviceAddress->SetLabel(InterfaceTypes::getAddressString(data->data64_1));
    distNodeDeviceAddress->SetLabel(InterfaceTypes::getAddressString(data->data64_2));

}

void UserInterface::distAddtoCollectorList(wxCommandEvent &event) {

    long i = 0;

    EventData *data = (EventData *)event.GetClientData();

    for (; i < distCollList->GetItemCount(); i++) {

        std::string item = distCollList->GetItemText(i, 0).ToStdString();
        std::string address = InterfaceTypes::getAddressString(data->data64_1);

        if (address.compare(item) == 0) {
            break;
        }

    }

    if (i == distCollList->GetItemCount()) {
        i = distCollList->InsertItem(distCollList->GetItemCount(), 0);
    }

    distCollList->SetItem(i, 0, InterfaceTypes::getAddressString(data->data64_1));
    distCollList->SetItem(i, 1, wxString::Format(wxT("%ld"), data->data64_1));

    if (data->data64_2 > 0) {
        distCollList->SetItem(i, 1, InterfaceTypes::getAddressString(data->data64_2));
    } else {
        distCollList->SetItem(i, 1, "No Available Node!!");
    }

}

void UserInterface::distAddtoNodeList(wxCommandEvent &event) {

    long i = 0;

    EventData *data = (EventData *)event.GetClientData();

    for (; i < distNodeList->GetItemCount(); i++) {

        std::string item = distNodeList->GetItemText(i, 0).ToStdString();
        std::string address = InterfaceTypes::getAddressString(data->data64_1);

        if (address.compare(item) == 0) {
            break;
        }

    }

    if (i == distNodeList->GetItemCount()) {
        i = distNodeList->InsertItem(distNodeList->GetItemCount(), 0);
    }

    distNodeList->SetItem(i, 0, InterfaceTypes::getAddressString(data->data64_1));
    distNodeList->SetItem(i, 1, NodeState::getName((NODE_STATES)data->data64_2));

}

void UserInterface::distUpdateBackup(wxCommandEvent &event) {

    EventData *data = (EventData *)event.GetClientData();

    distBackupStatus->SetLabelText(wxString::Format(wxT("%ld"), data->data64_1));

}

void UserInterface::distUpdateLog(wxCommandEvent &event) {

    EventData *data = (EventData *)event.GetClientData();

    distLog->Append(wxString::Format("%s", data->dataStr));
    LOG_S("%s : %s", ComponentTypes::getName(COMP_DISTRIBUTOR), data->dataStr.c_str());
}