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

    width = collProcessList->GetSize().GetWidth() - 1;

    column.SetId(0);
    column.SetText( _("Processes") );
    column.SetWidth(width);
    collProcessList->InsertColumn(0, column);

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

        long row = collProcessList->InsertItem(collProcessList->GetItemCount(), job->getOrderedExecution(j)->getExec());
        collProcessList->SetItemBackgroundColour(row, job->getOrderedExecution(j)->isValid() ?
                                                    wxColour(0, 255, 0) : wxColour(255, 255, 255));
    }
}

void Wx::collUpdateProcessListItem(wxCommandEvent &event) {

    auto *data = (UserInterfaceEvent *)event.GetClientData();

    auto *job = (Job *)data->getPointer(0);

    for (int j = 0; j < job->getOrderedCount(); j++) {

        switch(job->getOrderedState(j)) {

            case PROCESS_STATE_ENDED:
                collProcessList->SetItemBackgroundColour(j, wxColour(180, 180, 180));
                break;

            case PROCESS_STATE_STARTED:
                collProcessList->SetItemBackgroundColour(j, wxColour(255, 0, 255));
                break;

            case PROCESS_STATE_PROVISION:
                collProcessList->SetItemBackgroundColour(j, wxColour(0, 255, 0));
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


