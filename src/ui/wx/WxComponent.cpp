//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#include <Application.h>
#include "WxComponent.h"

void Wx::componentInit() {

    Log::init(LEVEL_INFO, this, updateUICallback);

    uiUpdater = new fUIUpdater[UI_UPDATE_MAX];

    ui_event = new wxEventTypeTag<wxCommandEvent>(wxNewEventType());
    Bind(*ui_event, &Wx::updateUI, this, 0, UI_UPDATE_MAX - 1, NULL);

    distInit();
    collInit();
    nodeInit();

    if (((Application*) app)->getStartState()) {

        interfaceInit->Enable(false);
        distCollInterface->Enable(false);
        nodeInterface->Enable(false);
        distInitBtn->Enable(true);
        collInitBtn->Enable(true);
        nodeInitBtn->Enable(true);
        return;
    }

    DeviceList *deviceList = DeviceList::getInstance();

    for (uint32_t i = 0; i < deviceList->getCount(); i++) {
        distCollInterface->Insert(wxString(InterfaceTypes::getName(deviceList->get(i)->getType())) +
                                  " --> " + deviceList->get(i)->getName(), i);
        nodeInterface->Insert(wxString(InterfaceTypes::getName(deviceList->get(i)->getType())) +
                              " --> " + deviceList->get(i)->getName(), i);
        if (deviceList->get(i)->isLoopback()) {
            distCollInterface->SetSelection(i);
            nodeInterface->SetSelection(i);
        }
    }
}

void Wx::OnInterfaceInitClickWrapper( wxCommandEvent& event )
{
    DeviceList::getInstance()->setActive(distCollInterface->GetSelection(),
                                         nodeInterface->GetSelection());

    interfaceInit->Enable(false);
    distCollInterface->Enable(false);
    nodeInterface->Enable(false);
    distInitBtn->Enable(true);
    collInitBtn->Enable(true);
    nodeInitBtn->Enable(true);
}

void Wx::updateUIEvent(int id, void *data) {

    wxCommandEvent event(*ui_event);
    event.SetId(id);
    event.SetClientData(data);
    wxPostEvent(this, event);
}

void Wx::updateUI(wxCommandEvent& event) {

    int id = event.GetId();

    if (uiUpdater[id] != nullptr) {
        ((this)->*(uiUpdater[id]))(event);
    }
}

void Wx::updateUICallback(void *context, int id, void *data) {
    ((Wx*) context)->updateUIEvent(id, data);
}
