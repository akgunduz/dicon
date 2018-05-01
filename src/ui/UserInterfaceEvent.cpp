//
// Created by Haluk AKGUNDUZ on 30.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "UserInterfaceEvent.h"

UserInterfaceEvent::UserInterfaceEvent(int id) : id(id) {

}

long UserInterfaceEvent::getData(int index) {

    return data[index];
}

std::string UserInterfaceEvent::getString(int index) {

    return str[index];
}

void *UserInterfaceEvent::getPointer(int index) {

    return pointer[index];
}

void UserInterfaceEvent::setID(int id) {

    this->id = id;
}

void UserInterfaceEvent::addData(long data) {

    this->data.push_back(data);
}

void UserInterfaceEvent::addDataList(std::vector<long> dataList) {

    this->data.insert(this->data.end(), dataList.begin(), dataList.end());
}

void UserInterfaceEvent::addString(std::string str) {

    this->str.push_back(str);
}

void UserInterfaceEvent::addPointer(void *pointer) {

    this->pointer.push_back(pointer);
}

long UserInterfaceEvent::getDataSize() {

    return data.size();
}
