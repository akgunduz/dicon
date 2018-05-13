//
// Created by Haluk AKGUNDUZ on 30.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "UserInterfaceController.h"

UserInterfaceController* UserInterfaceController::instance = NULL;

UserInterfaceController::UserInterfaceController(void *context, TypeUIUpdateCB updateCB) {

    this->uiContext = context;
    this->uiUpdateCB = updateCB;
}

UserInterfaceController *UserInterfaceController::newInstance(void *context, TypeUIUpdateCB updateCB) {

    if (!instance) {

        instance = new UserInterfaceController(context, updateCB);
    }

    return instance;
}

void UserInterfaceController::updateUI(int id, UserInterfaceEvent *event) {

    if (uiUpdateCB && uiContext) {
        uiUpdateCB(uiContext, id, event);
    }
}

void UserInterfaceController::display(int id, std::vector<long> dataList) {

    auto *event = new UserInterfaceEvent(id);

    event->addDataList(dataList);

    updateUI(id, event);
}


void UserInterfaceController::display(int id, char const *data1, ...) {

    char buf[256];
    va_list ap;
    va_start(ap, data1);
    vsnprintf(buf, sizeof(buf), data1, ap);
    va_end(ap);

    auto *event = new UserInterfaceEvent(id);

    event->addString(std::string(buf));

    updateUI(id, event);

}

void UserInterfaceController::display(int id, int data1, char const *data2, char const *data3) {

    auto *event = new UserInterfaceEvent(id);

    event->addData(data1);
    event->addString(std::string(data2));
    event->addString(std::string(data3));

    updateUI(id, event);

}

void UserInterfaceController::display(int id, int data1, long data2, char const *data3, char const *data4) {

    auto *event = new UserInterfaceEvent(id);

    event->addData(data1);
    event->addData(data2);
    event->addString(std::string(data3));
    event->addString(std::string(data4));

    updateUI(id, event);

}

void UserInterfaceController::display(int id, int data1, char const *data2, long data3) {

    auto *event = new UserInterfaceEvent(id);

    event->addData(data1);
    event->addString(std::string(data2));
    event->addData(data3);

    updateUI(id, event);
}

void UserInterfaceController::display(int id, std::vector<long> data1, const char *data2, ...) {

    char buf[256];
    va_list ap;
    va_start(ap, data2);
    vsnprintf(buf, sizeof(buf), data2, ap);
    va_end(ap);

    auto *event = new UserInterfaceEvent(id);

    event->addDataList(data1);
    event->addString(std::string(buf));

    updateUI(id, event);

}

void UserInterfaceController::display(int id, void *data1) {

    auto *event = new UserInterfaceEvent(id);

    event->addPointer(data1);

    updateUI(id, event);

}

void UserInterfaceController::display(int id, int data1, void *data2) {

    auto *event = new UserInterfaceEvent(id);

    event->addData(data1);
    event->addPointer(data2);

    updateUI(id, event);

}