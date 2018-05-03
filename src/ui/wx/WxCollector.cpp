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

void Wx::collInit() {

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

    uiUpdater[UI_UPDATE_COLL_ADDRESS] = &Wx::collUpdateAddresses;
    uiUpdater[UI_UPDATE_COLL_ATT_DIST_ADDRESS] = &Wx::collUpdateAttachedDistAddress;
    uiUpdater[UI_UPDATE_COLL_ATT_NODE_ADDRESS] = &Wx::collUpdateAttachedNodeAddress;
    uiUpdater[UI_UPDATE_COLL_FILE_LIST] = &Wx::collUpdateFileList;
    uiUpdater[UI_UPDATE_COLL_FILE_LISTITEM] = &Wx::collUpdateFileListItem;
    uiUpdater[UI_UPDATE_COLL_PROCESS_LIST] = &Wx::collUpdateProcessList;
}

/*
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_COLL_INIT
 */

void Wx::OnCollInitClickWrapper( wxCommandEvent& event )
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

void Wx::OnCollProcessClickWrapper( wxCommandEvent& event )
{
    ((Collector*)collObject)->processJobs();
}

void Wx::collUpdateAddresses(wxCommandEvent &event) {

    auto *data = (UserInterfaceEvent *)event.GetClientData();

    collDistDeviceAddress->SetLabel(InterfaceTypes::getAddressString(data->getData(0)));
    collNodeDeviceAddress->SetLabel(InterfaceTypes::getAddressString(data->getData(1)));

}

void Wx::collUpdateAttachedDistAddress(wxCommandEvent &event) {

    auto *data = (UserInterfaceEvent *)event.GetClientData();

    collDistAddress->SetLabel(InterfaceTypes::getAddressString(data->getData(0)));

}

void Wx::collUpdateAttachedNodeAddress(wxCommandEvent &event) {

    auto *data = (UserInterfaceEvent *)event.GetClientData();

    collNodeAddress->SetLabel(InterfaceTypes::getAddressString(data->getData(0)));
}

void Wx::collUpdateFileList(wxCommandEvent &event) {

    auto *data = (UserInterfaceEvent *)event.GetClientData();

    auto *job = (Job*)data->getPointer(0);

    for (int j = 0; j < job->getFileCount(); j++) {

        auto *content = job->getFile(j);

        long row = collFileList->InsertItem(collFileList->GetItemCount(), content->getFileName());

        if (content->isValid()) {
            collFileList->SetItemBackgroundColour(row, wxColour(0, 255, 0));
        }
    }
}

void Wx::collUpdateFileListItem(wxCommandEvent &event) {

    auto *data = (UserInterfaceEvent *)event.GetClientData();

    for (int i = 0; i < data->getDataSize(); i++) {

        collFileList->SetItemBackgroundColour(data->getData(i), wxColour(0, 255, 0));
    }
}

void Wx::collUpdateProcessList(wxCommandEvent &event) {

    auto *data = (UserInterfaceEvent *)event.GetClientData();

    auto *job = (Job *)data->getPointer(0);

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

