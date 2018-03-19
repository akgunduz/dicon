//
// Created by Haluk Akgunduz on 02/09/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Console.h"

bool Console::nodeInit(INTERFACES distInterface, INTERFACES collInterface) {

	uiUpdater[UI_UPDATE_NODE_ADDRESS] = &Console::nodeUpdateAddresses;
	uiUpdater[UI_UPDATE_NODE_STATE] = &Console::nodeUpdateState;
	uiUpdater[UI_UPDATE_NODE_ATT_COLL_ADDRESS] = &Console::nodeUpdateAttachedCollAddress;
	uiUpdater[UI_UPDATE_NODE_FILE_LIST] = &Console::nodeUpdateFileList;
	uiUpdater[UI_UPDATE_NODE_PARAM_LIST] = &Console::nodeUpdateParamList;
	uiUpdater[UI_UPDATE_NODE_EXEC_LIST] = &Console::nodeUpdateExecList;
	uiUpdater[UI_UPDATE_NODE_LOG] = &Console::nodeUpdateLog;

	char path[PATH_MAX];
	sprintf(path, "%s/%s", getcwd(nullptr, 0), NODE_PATH);

	nodeRemoveDir(path);
	mkdir(path, 0777);

	nodeObject = nullptr;

	try {

		nodeObject = new Node(distInterface, collInterface, path);

	} catch (std::runtime_error &e) {

		return false;
	}

	return true;

}

void Console::nodeDestroy() {

	if (nodeObject) {
		delete nodeObject;
	}

}

void Console::nodeRemoveDir(const char *dirpath) {

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
				nodeRemoveDir(path);
			} else {
				unlink(path);
			}

		}

	}
	closedir(dir);

	rmdir(dirpath);
}

void Console::nodeRun(INTERFACES distInterface, INTERFACES collInterface) {

	if (!nodeInit(distInterface, collInterface) ) {
		nodeDestroy();
		return;
	}

	int in;
	do {

		in = getchar();
		switch(in) {
			case 'q':
				nodeDestroy();
				return;
			case 'd':
				Log::iterateLogLevel();
				break;
			default:
				break;
		}

	} while(1);

}

void Console::nodeUpdateAddresses(ConsoleEvent &event) {

	EventData *data = (EventData *) event.GetClientData();
	LOG_S("Node Interface Addresses --> Distributor : %s, Collector : %s",
		  Address::getString(data->data64_1).c_str(),
		  Address::getString(data->data64_2).c_str());

}

void Console::nodeUpdateState(ConsoleEvent &event) {

	EventData *data = (EventData *) event.GetClientData();
	LOG_S("Node State : %s", sStates[data->data64_1]);

}

void Console::nodeUpdateAttachedCollAddress(ConsoleEvent &event) {

	EventData *data = (EventData *) event.GetClientData();
	LOG_S("Node Attached Collector : %s", Address::getString(data->data64_1).c_str());

}

void Console::nodeUpdateLog(ConsoleEvent &event) {

	EventData *data = (EventData *) event.GetClientData();
	LOG_S("%s", data->dataStr.c_str());

}

void Console::nodeUpdateFileList(ConsoleEvent &event) {

	Rule *rule = (Rule *) event.GetClientData();

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

void Console::nodeUpdateParamList(ConsoleEvent &event) {

	Rule *rule = (Rule *) event.GetClientData();

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

void Console::nodeUpdateExecList(ConsoleEvent &event) {

	Rule *rule = (Rule *) event.GetClientData();

	for (int j = 0; j < rule->getContentCount(CONTENT_EXECUTOR); j++) {

		ExecutorItem *content = (ExecutorItem *) rule->getContent(CONTENT_EXECUTOR, j);
		if (content == nullptr) {
			return;
		}

		LOG_S("Node Executor : %s", content->getExec());

	}
}
