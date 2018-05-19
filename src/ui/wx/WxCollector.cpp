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

#include <Application.h>
#include <ComponentController.h>
#include "WxComponent.h"

void Wx::collInit(bool enable) {

    if (!enable) {

        mainPanel->GetPage(1)->Hide();
        return;
    }

    int width = collFileList->GetSize().GetWidth() - 1;

    wxListItem column;
    column.SetId(0);
    column.SetText( _("Files") );
    column.SetWidth(width);
    collFileList->InsertColumn(0, column);

    width = collProcessList->GetSize().GetWidth() / 9;

    column.SetId(0);
    column.SetText( _("ID") );
    column.SetWidth(width * 1);
    collProcessList->InsertColumn(0, column);

    column.SetId(1);
    column.SetText( _("Node") );
    column.SetWidth(width * 1);
    collProcessList->InsertColumn(1, column);

    column.SetId(2);
    column.SetText( _("Processes") );
    column.SetWidth(width * 7);
    collProcessList->InsertColumn(2, column);

    uiUpdater[UI_UPDATE_COLL_ID] = &Wx::collUpdateID;
    uiUpdater[UI_UPDATE_COLL_FILE_LIST] = &Wx::collUpdateFileList;
    uiUpdater[UI_UPDATE_COLL_FILE_LISTITEM] = &Wx::collUpdateFileListItem;
    uiUpdater[UI_UPDATE_COLL_PROCESS_LIST] = &Wx::collUpdateProcessList;
    uiUpdater[UI_UPDATE_COLL_PROCESS_LISTITEM] = &Wx::collUpdateProcessListItem;
}

void Wx::OnCollLoadClickWrapper( wxCommandEvent& event )
{
    collFileList->DeleteAllItems();
    collProcessList->DeleteAllItems();

    ((ComponentController *)componentController)->getCollector(0)->loadJob(NULL);
}

void Wx::OnCollProcessClickWrapper( wxCommandEvent& event )
{
    ((ComponentController *)componentController)->getCollector(0)->processJobs();
}

void Wx::collUpdateID(wxCommandEvent &event) {

    auto *data = (UserInterfaceEvent *)event.GetClientData();

    mainPanel->SetPageText(1, wxString::Format(wxT("Collector[%ld]"), data->getData(0)));
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

    for (int j = 0; j < job->getOrderedCount(); j++) {

        long row = collProcessList->InsertItem(collProcessList->GetItemCount(), 0);

        collProcessList->SetItem(row, 0, wxString::Format(wxT("%d"), j));
        collProcessList->SetItem(row, 1, wxString::Format(wxT("%ld"), data->getData(0)));
        collProcessList->SetItem(row, 2, job->getOrderedProcess(j)->getProcess());
        collProcessList->SetItemBackgroundColour(row, job->getOrderedProcess(j)->isValid() ?
                                                    wxColour(0, 255, 0) : wxColour(255, 255, 255));
    }
}

void Wx::collUpdateProcessListItem(wxCommandEvent &event) {

    auto *data = (UserInterfaceEvent *)event.GetClientData();

    auto processID = data->getData(0);
    auto processState = data->getData(1);
    auto nodeID = data->getData(2);

    if (nodeID > 0) {
        collProcessList->SetItem(processID, 1, wxString::Format(wxT("%ld"), nodeID));
    }

    switch(processState) {

            case PROCESS_STATE_ENDED:
                collProcessList->SetItemBackgroundColour(processID, wxColour(180, 180, 180));
                break;

            case PROCESS_STATE_STARTED:
                collProcessList->SetItemBackgroundColour(processID, wxColour(255, 0, 255));
                break;

            case PROCESS_STATE_NOTSTARTED:
                collProcessList->SetItemBackgroundColour(processID, wxColour(255, 255, 255));
                break;

            default:
                break;
        }
}


