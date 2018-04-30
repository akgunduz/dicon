//
// Created by Haluk Akgunduz on 02/09/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Console.h"

Console::Console() {

    Log::setLogLevel(LEVEL_INFO);
    Log::set_ui_callback(this, updateUICallback);
}

ConsoleEvent::ConsoleEvent() {
	id = 0;
	data64_1 = 0;
	data64_2 = 0;
	dataStr = "";
	dataPointer = nullptr;
}

uint64_t ConsoleEvent::GetData1() {
	return data64_1;
}

uint64_t ConsoleEvent::GetData2() {
	return data64_2;
}

std::string ConsoleEvent::GetString() {
	return dataStr;
}

void *ConsoleEvent::GetClientData() {
	return dataPointer;
}

void ConsoleEvent::SetId(int id) {
	this->id = id;
}

void ConsoleEvent::SetData1(uint64_t data64) {
	this->data64_1 = data64;
}

void ConsoleEvent::SetData2(uint64_t data64) {
	this->data64_2 = data64;
}

void ConsoleEvent::SetString(std::string &dataStr) {
	this->dataStr = dataStr;
}

void ConsoleEvent::SetClientData(void *dataPointer) {
	this->dataPointer = dataPointer;
}

void Console::updateUIEvent(int id, void *data) {

	ConsoleEvent event;
	event.SetId(id);
    event.SetClientData(data);
	if (uiUpdater[id] != nullptr) {
		((this)->*(uiUpdater[id]))(event);
	}
}

void Console::updateUICallback(void *context, int id, void *data) {
    ((Console*) context)->updateUIEvent(id, data);
}
