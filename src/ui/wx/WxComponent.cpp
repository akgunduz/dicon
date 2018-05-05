//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#include <ComponentController.h>
#include "WxComponent.h"

void Wx::componentInit() {

    Log::registerUIController(this, updateUICallback);

    uiUpdater = new fUIUpdater[UI_UPDATE_MAX];

    ui_event = new wxEventTypeTag<wxCommandEvent>(wxNewEventType());
    Bind(*ui_event, &Wx::updateUI, this, 0, UI_UPDATE_MAX - 1, NULL);

    distInit();
    collInit();
    nodeInit();

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
