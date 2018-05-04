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

void Wx::nodeInit() {

    int width = nodeProcessList->GetSize().GetWidth() / 12 - 1;

    wxListItem column;
    column.SetId(0);
    column.SetText( _("Collector ID") );
    column.SetWidth(width * 2);
    nodeProcessList->InsertColumn(0, column);

    column.SetId(1);
    column.SetText( _("Job Name") );
    column.SetWidth(width * 3);
    nodeProcessList->InsertColumn(1, column);

    column.SetId(2);
    column.SetText( _("Process") );
    column.SetWidth(width * 7);
    nodeProcessList->InsertColumn(2, column);

    nodeState->SetLabel(NodeState::getName(IDLE));

    uiUpdater[UI_UPDATE_NODE_STATE] = &Wx::nodeUpdateState;
    uiUpdater[UI_UPDATE_NODE_PROCESS_LIST] = &Wx::nodeUpdateProcessList;
    uiUpdater[UI_UPDATE_NODE_CLEAR] = &Wx::nodeUpdateClear;

}

void Wx::nodeUpdateState(wxCommandEvent &event) {

    auto *data = (UserInterfaceEvent *)event.GetClientData();

    nodeState->SetLabel(NodeState::getName((NODE_STATES)data->getData(0)));
}

void Wx::nodeUpdateClear(wxCommandEvent &event) {

    nodeProcessList->DeleteAllItems();
}

void Wx::nodeUpdateProcessList(wxCommandEvent &event) {

//    auto *data = (UserInterfaceEvent *)event.GetClientData();
//
//    nodeExecList->InsertItem(nodeExecList->GetItemCount(), wxString::Format("%s", data->getString(0)));
}
