//
// Created by Haluk AKGUNDUZ on 6.05.2018.
//

#include "ConsoleEvent.h"

ConsoleEvent::ConsoleEvent() {
    id = 0;
    dataPointer = nullptr;
}

void *ConsoleEvent::GetClientData() {
    return dataPointer;
}

int ConsoleEvent::GetId() {
    return id;
}

void ConsoleEvent::SetId(int id) {
    this->id = id;
}

void ConsoleEvent::SetClientData(void *dataPointer) {
    this->dataPointer = dataPointer;
}
