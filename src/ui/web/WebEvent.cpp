//
// Created by Haluk AKGUNDUZ on 23.06.2020.
//

#include "WebEvent.h"

WebEvent::WebEvent() {
    id = 0;
    dataPointer = nullptr;
}

void *WebEvent::GetClientData() {
    return dataPointer;
}

int WebEvent::GetId() {
    return id;
}

void WebEvent::SetId(int id) {
    this->id = id;
}

void WebEvent::SetClientData(void *dataPointer) {
    this->dataPointer = dataPointer;
}
