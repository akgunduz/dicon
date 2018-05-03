//
// Created by Haluk Akgunduz on 02/09/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Console.h"

bool Console::nodeInit(int distInterface, int collInterface) {

	uiUpdater[UI_UPDATE_NODE_ADDRESS] = &Console::nodeUpdateAddresses;
	uiUpdater[UI_UPDATE_NODE_STATE] = &Console::nodeUpdateState;
	uiUpdater[UI_UPDATE_NODE_ATT_COLL_ADDRESS] = &Console::nodeUpdateAttachedCollAddress;
	uiUpdater[UI_UPDATE_NODE_FILE_LIST] = &Console::nodeUpdateFileList;
	uiUpdater[UI_UPDATE_NODE_EXEC_LIST] = &Console::nodeUpdateExecList;
	uiUpdater[UI_UPDATE_NODE_LOG] = &Console::nodeUpdateLog;

	char path[PATH_MAX];
	sprintf(path, "%s/%s", getcwd(nullptr, 0), NODE_PATH);

	nodeRemoveDir(path);
	mkdir(path, 0777);

	try {

		DeviceList::getInstance()->setActive(distInterface, collInterface);

		nodeObject = new Node(path);

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

void Console::nodeRun(int distInterface, int collInterface) {

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
          InterfaceTypes::getAddressString(data->data64_1).c_str(),
          InterfaceTypes::getAddressString(data->data64_2).c_str());

}

void Console::nodeUpdateState(ConsoleEvent &event) {

	EventData *data = (EventData *) event.GetClientData();
    LOG_S("Node State : %s", NodeState::getName((NODE_STATES) data->data64_1));

}

void Console::nodeUpdateAttachedCollAddress(ConsoleEvent &event) {

	EventData *data = (EventData *) event.GetClientData();
    LOG_S("Node Attached Collector : %s", InterfaceTypes::getAddressString(data->data64_1).c_str());

}

void Console::nodeUpdateLog(ConsoleEvent &event) {

	EventData *data = (EventData *) event.GetClientData();
    LOG_S("%s", data->dataStr.c_str());

}

void Console::nodeUpdateFileList(ConsoleEvent &event) {

	Job *job = (Job *)event.GetClientData();

	for (int j = 0; j < job->getContentCount(CONTENT_FILE); j++) {

		FileItem *content = (FileItem *) job->getContent(CONTENT_FILE, j);
		if (content == nullptr) {
			return;
		}

		long i = 0;

        LOG_S("Node File --> Path : %s, MD5 : %s",
              content->getFileName(), content->getMD5()->getStr().c_str());
	}
}

void Console::nodeUpdateExecList(ConsoleEvent &event) {

    EventData *data = (EventData *)event.GetClientData();
    LOG_S("Node Executor : %s", data->dataStr.c_str());
}
