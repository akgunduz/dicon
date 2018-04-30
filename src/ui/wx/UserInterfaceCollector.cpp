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

#include "UserInterfaceComponent.h"

void UserInterface::collInit() {

    int width = collFileList->GetSize().GetWidth() - 1;

    wxListItem column;
    column.SetId(0);
    column.SetText( _("Files") );
    column.SetWidth(width);
    collFileList->InsertColumn(0, column);

    width = collProcessList->GetSize().GetWidth() - 1;

    column.SetId(0);
    column.SetText( _("Processes") );
    column.SetWidth(width);
    collProcessList->InsertColumn(0, column);

    uiUpdater[UI_UPDATE_COLL_ADDRESS] = &UserInterface::collUpdateAddresses;
    uiUpdater[UI_UPDATE_COLL_ATT_DIST_ADDRESS] = &UserInterface::collUpdateAttachedDistAddress;
    uiUpdater[UI_UPDATE_COLL_ATT_NODE_ADDRESS] = &UserInterface::collUpdateAttachedNodeAddress;
    uiUpdater[UI_UPDATE_COLL_FILE_LIST] = &UserInterface::collUpdateFileList;
    uiUpdater[UI_UPDATE_COLL_FILE_LISTITEM] = &UserInterface::collUpdateFileListItem;
    uiUpdater[UI_UPDATE_COLL_PROCESS_LIST] = &UserInterface::collUpdateProcessList;
}

/*
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_COLL_INIT
 */

void UserInterface::OnCollInitClickWrapper( wxCommandEvent& event )
{
    if (wxStrcmp(collInitBtn->GetLabel(), "Init") == 0) {

        try {

            collObject = Collector::newInstance(1);

        } catch (std::runtime_error &e) {

            return;
        }

        collProcessBtn->Enable(true);
        collInitBtn->SetLabel("Stop");

    } else {

        delete ((Collector*)collObject);
        collProcessBtn->Enable(false);
        collInitBtn->SetLabel("Init");
        collDistDeviceAddress->SetLabel("");
        collNodeDeviceAddress->SetLabel("");
        collFileList->ClearAll ();
        collProcessList->ClearAll ();
    }
}


/*
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_COLL_PROCESS
 */

void UserInterface::OnCollProcessClickWrapper( wxCommandEvent& event )
{
    ((Collector*)collObject)->processJobs();
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

void UserInterface::collUpdateFileList(wxCommandEvent &event) {

    auto *job = (Job*)event.GetClientData();

    for (int j = 0; j < job->getFileCount(); j++) {

        auto *content = job->getFile(j);

        long row = collFileList->InsertItem(collFileList->GetItemCount(), content->getFileName());

        if (content->isValid()) {
            collFileList->SetItemBackgroundColour(row, wxColour(0, 255, 0));
        }
    }
}

void UserInterface::collUpdateFileListItem(wxCommandEvent &event) {

    auto *data = (EventData *)event.GetClientData();

    for (int i = 0; i < data->data64_list.size(); i++) {

        collFileList->SetItemBackgroundColour(data->data64_list[i], wxColour(0, 255, 0));
    }
}

void UserInterface::collUpdateProcessList(wxCommandEvent &event) {

    auto *job = (Job *)event.GetClientData();

    if (job->getOrderedCount() != collProcessList->GetItemCount()) {

        for (int j = 0; j < job->getOrderedCount(); j++) {

            collProcessList->InsertItem(collProcessList->GetItemCount(), job->getOrderedExecution(j)->getExec());
        }
    }

    for (int j = 0; j < job->getOrderedCount(); j++) {

        switch(job->getOrderedState(j)) {

            case PROCESS_STATE_ENDED:
                collProcessList->SetItemBackgroundColour(j, wxColour(180, 180, 180));
                break;

            case PROCESS_STATE_STARTED:
                collProcessList->SetItemBackgroundColour(j, wxColour(255, 0, 255));
                break;

            case PROCESS_STATE_NOTSTARTED:
                collProcessList->SetItemBackgroundColour(j, job->getOrderedExecution(j)->isValid() ?
                                                            wxColour(0, 255, 0) : wxColour(255, 255, 255));
                break;

            default:
                break;
        }
    }
}

