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

void Wx::nodeInit(bool enable) {

    if (!enable) {

        mainPanel->GetPage(2)->Hide();
        return;
    }

    int width = nodeProcessList->GetSize().GetWidth() / 8;

    wxListItem column;
    column.SetId(0);
    column.SetText( _("Collector ID") );
    column.SetWidth(width * 1);
    nodeProcessList->InsertColumn(0, column);

    column.SetId(1);
    column.SetText( _("Job ID") );
    column.SetWidth(width * 2);
    nodeProcessList->InsertColumn(1, column);

    column.SetId(2);
    column.SetText( _("Job Name") );
    column.SetWidth(width * 4);
    nodeProcessList->InsertColumn(2, column);

    column.SetId(3);
    column.SetText( _("Process ID") );
    column.SetWidth(width * 1);
    nodeProcessList->InsertColumn(3, column);

    nodeState->SetLabel(NodeState::getName(IDLE));

    uiUpdater[UI_UPDATE_NODE_ID] = &Wx::nodeUpdateID;
    uiUpdater[UI_UPDATE_NODE_STATE] = &Wx::nodeUpdateState;
    uiUpdater[UI_UPDATE_NODE_PROCESS_LIST] = &Wx::nodeUpdateProcessList;
    uiUpdater[UI_UPDATE_NODE_CLEAR] = &Wx::nodeUpdateClear;
}

void Wx::nodeUpdateID(wxCommandEvent &event) {

    auto *data = (UserInterfaceEvent *)event.GetClientData();

    mainPanel->SetPageText(2, wxString::Format(wxT("Node[%ld]"), data->getData(0)));
}

void Wx::nodeUpdateState(wxCommandEvent &event) {

    auto *data = (UserInterfaceEvent *)event.GetClientData();

    nodeState->SetLabel(NodeState::getName((NODE_STATES)data->getData(0)));
}

void Wx::nodeUpdateClear(wxCommandEvent &event) {

    nodeProcessList->DeleteAllItems();
}

void Wx::nodeUpdateProcessList(wxCommandEvent &event) {

    auto *data = (UserInterfaceEvent *)event.GetClientData();

    long row = nodeProcessList->InsertItem(nodeProcessList->GetItemCount(), 0);

    nodeProcessList->SetItem(row, 0, wxString::Format(wxT("%ld"), data->getData(0)));
    nodeProcessList->SetItem(row, 1, data->getString(0));
    nodeProcessList->SetItem(row, 2, wxString::Format(wxT("%ld"), data->getData(1)));
}
