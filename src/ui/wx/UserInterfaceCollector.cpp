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

//    int width = collJobList->GetSize().GetWidth() / 5.5;
//
//    collJobList->AppendColumn("ID", width * 4, wxALIGN_LEFT, 0);
//    collJobList->AppendColumn("Repeat", width, wxALIGN_RIGHT, 0);

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

void UserInterface::collUpdateFileList(wxCommandEvent &event) {

    auto *job = (Job*)event.GetClientData();

    TypeFileInfoList list;
    for (int j = 0; j < job->getOrderedCount(); j++) {

        TypeFileInfoList ins = FileInfo::getFileList(job->getOrdered(j)->getFileList(), FILEINFO_EXIST);
        list.insert(list.end(), ins.begin(), ins.end());
    }

    for (int j = 0; j < job->getFileCount(); j++) {

        auto *content = job->getFile(j);
        if (content == nullptr) {
            return;
        }

        int i = 0;

        for (; i < collFileList->GetItemCount(); i++) {

            std::string file = collFileList->GetItemText(i, 0).ToStdString();

            if (file.compare(content->getFileName()) == 0) {
                break;
            }
        }

        if (i == collFileList->GetItemCount()) {
            collFileList->InsertItem(collFileList->GetItemCount(), content->getFileName());
        }

        if (FileInfo::isInclude(&list, content)) {
            collFileList->SetItemBackgroundColour(j, wxColour(0, 255, 0));
        }
    }
}

void UserInterface::collUpdateFileListItem(wxCommandEvent &event) {

    EventData *data = (EventData *)event.GetClientData();

}

void UserInterface::collUpdateProcessList(wxCommandEvent &event) {

    auto *job = (Job *)event.GetClientData();

    TypeFileInfoList list;
    for (int j = 0; j < job->getIndependentCount(); j++) {

        TypeFileInfoList ins = FileInfo::getFileList(job->getIndependent(j)->getFileList(), FILEINFO_EXIST);
        list.insert(list.end(), ins.begin(), ins.end());
    }


    for (int j = 0; j < job->getOrderedCount(); j++) {

        auto *content = job->getOrdered(j);
        if (content == NULL) {
            return;
        }

        collProcessList->InsertItem(collProcessList->GetItemCount(), content->getExec());
        if (job->isIndependent(content)) {
            collProcessList->SetItemBackgroundColour(j, wxColour(0, 255, 0));
        }
    }

}