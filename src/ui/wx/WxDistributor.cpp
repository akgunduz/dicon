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

#include <ComponentController.h>
#include "WxComponent.h"

void Wx::distInit() {

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

    uiUpdater[UI_UPDATE_DIST_ADDRESS] = &Wx::distUpdateAddresses;
    uiUpdater[UI_UPDATE_DIST_COLL_LIST] = &Wx::distAddtoCollectorList;
    uiUpdater[UI_UPDATE_DIST_NODE_LIST] = &Wx::distAddtoNodeList;
}

void Wx::OnDistPollClickWrapper( wxCommandEvent& event )
{
    distCollList->DeleteAllItems();
    distNodeList->DeleteAllItems();

    ((ComponentController *)componentController)->getDistributor()->reset();
    ((ComponentController *)componentController)->getDistributor()->sendWakeupMessagesAll();
}

void Wx::distUpdateAddresses(wxCommandEvent &event) {

    auto *data = (UserInterfaceEvent *)event.GetClientData();

    distCollDeviceAddress->SetLabel(InterfaceTypes::getAddressString(data->getData(0)));
    distNodeDeviceAddress->SetLabel(InterfaceTypes::getAddressString(data->getData(1)));
}

void Wx::distAddtoCollectorList(wxCommandEvent &event) {

    long i = 0;

    auto *data = (UserInterfaceEvent *)event.GetClientData();

    for (; i < distCollList->GetItemCount(); i++) {

        std::string item = distCollList->GetItemText(i, 0).ToStdString();
        std::string address = InterfaceTypes::getAddressString(data->getData(0));

        if (address.compare(item) == 0) {
            break;
        }

    }

    if (i == distCollList->GetItemCount()) {
        i = distCollList->InsertItem(distCollList->GetItemCount(), 0);
    }

    distCollList->SetItem(i, 0, InterfaceTypes::getAddressString(data->getData(0)));
    distCollList->SetItem(i, 1, wxString::Format(wxT("%ld"), data->getData(0)));

    if (data->getData(1) > 0) {
        distCollList->SetItem(i, 1, InterfaceTypes::getAddressString(data->getData(1)));
    } else {
        distCollList->SetItem(i, 1, "No Available Node!!");
    }

}

void Wx::distAddtoNodeList(wxCommandEvent &event) {

    long i = 0;

    auto *data = (UserInterfaceEvent *)event.GetClientData();

    for (; i < distNodeList->GetItemCount(); i++) {

        std::string item = distNodeList->GetItemText(i, 0).ToStdString();
        std::string address = InterfaceTypes::getAddressString(data->getData(0));

        if (address == item) {
            break;
        }

    }

    if (i == distNodeList->GetItemCount()) {
        i = distNodeList->InsertItem(distNodeList->GetItemCount(), 0);
    }

    distNodeList->SetItem(i, 0, InterfaceTypes::getAddressString(data->getData(0)));
    distNodeList->SetItem(i, 1, NodeState::getName((NODE_STATES)data->getData(1)));

}

