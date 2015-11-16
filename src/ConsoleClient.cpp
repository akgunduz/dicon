//
// Created by Haluk Akgunduz on 02/09/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Console.h"
#include "ParameterItem.h"
#include "ExecutorItem.h"

bool Console::clientInit(INTERFACES distInterface, INTERFACES collInterface) {

	uiUpdater[UI_UPDATE_CLIENT_ADDRESS] = &Console::clientUpdateAddresses;
	uiUpdater[UI_UPDATE_CLIENT_STATE] = &Console::clientUpdateState;
	uiUpdater[UI_UPDATE_CLIENT_ATT_COLL_ADDRESS] = &Console::clientUpdateAttachedCollAddress;
	uiUpdater[UI_UPDATE_CLIENT_FILE_LIST] = &Console::clientUpdateFileList;
	uiUpdater[UI_UPDATE_CLIENT_PARAM_LIST] = &Console::clientUpdateParamList;
	uiUpdater[UI_UPDATE_CLIENT_EXEC_LIST] = &Console::clientUpdateExecList;
	uiUpdater[UI_UPDATE_CLIENT_LOG] = &Console::clientUpdateLog;

	char path[PATH_MAX];
	sprintf(path, "%s/%s/", getcwd(nullptr, 0), CLIENT_PATH);

	clientRemoveDir(path);
	mkdir(path, 0777);

	clientObject = nullptr;

	try {

		clientObject = new Node(distInterface, collInterface, path);

	} catch (std::runtime_error &e) {

		return false;
	}

	return true;

}

void Console::clientDestroy() {

	if (clientObject) {
		delete clientObject;
	}

}

void Console::clientRemoveDir(const char *dirpath) {

	struct dirent *entry;
	char path[PATH_MAX];

	DIR *dir = opendir(dirpath);
	if (dir == nullptr) {
		return;
	}

	while ((entry = readdir(dir)) != nullptr) {
		if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
			snprintf(path, (size_t) PATH_MAX, "%s/%s", dirpath, entry->d_name);
            if (entry->d_type == DT_DIR) {
				clientRemoveDir(path);
			} else {
				unlink(path);
			}

		}

	}
	closedir(dir);

	rmdir(dirpath);
}

void Console::clientRun(INTERFACES distInterface, INTERFACES collInterface) {

	if (!clientInit(distInterface, collInterface) ) {
		clientDestroy();
		return;
	}

	int in;
	do {

		in = getchar();
		switch(in) {
			case 'q':
				clientDestroy();
				return;
			case 'd':
				Log::iterateLogLevel();
				break;
			default:
				break;
		}

	} while(1);

}

void Console::clientUpdateAddresses(ConsoleEvent &event) {

	EventData *data = (EventData *)event.GetClientData();
	LOG_S("Node Interface Addresses --> Distributor : %s, Collector : %s",
		  Address::getString(data->data64_1).c_str(),
		  Address::getString(data->data64_2).c_str());

}

void Console::clientUpdateState(ConsoleEvent &event) {

	EventData *data = (EventData *)event.GetClientData();
	LOG_S("Node State : %s", sStates[data->data64_1]);

}

void Console::clientUpdateAttachedCollAddress(ConsoleEvent &event) {

	EventData *data = (EventData *)event.GetClientData();
	LOG_S("Node Attached Collector : %s", Address::getString(data->data64_1).c_str());

}

void Console::clientUpdateLog(ConsoleEvent &event) {

	EventData *data = (EventData *)event.GetClientData();
	LOG_S("%s", data->dataStr.c_str());

}

void Console::clientUpdateFileList(ConsoleEvent &event) {

	Rule *rule = (Rule *)event.GetClientData();

	for (int j = 0; j < rule->getContentCount(CONTENT_FILE); j++) {

		FileItem *content = (FileItem *) rule->getContent(CONTENT_FILE, j);
		if (content == nullptr) {
			return;
		}

		LOG_S("Node File --> Path : %s, MD5 : %s, Validity : %s, Flag : %s",
				content->getFileName(), content->getMD5()->getStr().c_str(),
				content->isValid() ? "V" : "I", content->isFlaggedToSent() ? "Y" : "N");
	}

}

void Console::clientUpdateParamList(ConsoleEvent &event) {

	Rule *rule = (Rule *)event.GetClientData();

	for (int j = 0; j < rule->getContentCount(CONTENT_PARAM); j++) {

		ParameterItem *content = (ParameterItem *) rule->getContent(CONTENT_PARAM, j);
		if (content == nullptr) {
			return;
		}

		switch(content->getParamType()) {
			case PARAM_LONG:
				LOG_S("Node Parameter --> Type : %s, Value : %ld",
						sParamTypes[content->getParamType()], content->getParam().latom);
				break;
			case PARAM_DOUBLE:
				LOG_S("Node Parameter --> Type : %s, Value : %lf",
						sParamTypes[content->getParamType()], content->getParam().datom);
				break;
			case PARAM_STRING:
				LOG_S("Node Parameter --> Type : %s, Value : %s",
						sParamTypes[content->getParamType()], content->getParam().sPtr);
				break;
			default:
				break;
		}
	}
}

void Console::clientUpdateExecList(ConsoleEvent &event) {

	Rule *rule = (Rule *)event.GetClientData();

	for (int j = 0; j < rule->getContentCount(CONTENT_EXECUTOR); j++) {

		ExecutorItem *content = (ExecutorItem *) rule->getContent(CONTENT_EXECUTOR, j);
		if (content == nullptr) {
			return;
		}

		LOG_S("Node Executor : %s", content->getExec());

	}
}
