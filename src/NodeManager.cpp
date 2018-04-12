//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "NodeManager.h"

NodeManager::NodeManager(Component* component,
						 fTimeoutCB timeoutCB, fWakeupCB wakeupCB, double backupRate)
{

    this->component = component;
	this->timeoutCB = timeoutCB;

#ifndef DISABLE_BACKUP
	this->backupRate = backupRate;
    this->readyBackup = 0;
    this->totalBackup = 0;
#endif

#ifndef DISABLE_RECOVERY
    this->wakeupCB = wakeupCB;
	nodeWatchdog = new NodeWatchdog(component, wakeupCB);
#endif
};

NodeManager::~NodeManager() {

#ifndef DISABLE_RECOVERY
    if (nodeWatchdog != nullptr) {
        delete nodeWatchdog;
    }
#endif
	for (TypeNodeList::iterator i = nodes.begin(); i != nodes.end(); i++) {
		NodeObject* node = i->second;
		delete node;
	}
}

NodeObject *NodeManager::getIdle() {

//TODO coklu collector de burada thread korumasi olmasi lazim

    NodeObject *leastUsedNode = NULL;

    int idleCount = 0;

    for (TypeNodeList::iterator i = nodes.begin(); i != nodes.end(); i++) {
        NodeObject *node = i->second;
        if (node->getState() != IDLE) {
            continue;
        }

        idleCount++;

        if (leastUsedNode == NULL) {
            leastUsedNode = node;
            continue;
        }

        if (node->getUsage() < leastUsedNode->getUsage()) {
            leastUsedNode = node;
        }
    }

    if (leastUsedNode != NULL
#ifndef DISABLE_BACKUP
        && idleCount - totalBackup + readyBackup > 0
#endif
            ) {
        leastUsedNode->setState(PREBUSY);

#ifndef DISABLE_RECOVERY
        if (leastUsedNode->watchdog != nullptr) {
            delete leastUsedNode->watchdog;
            leastUsedNode->watchdog = nullptr;
        }
        leastUsedNode->watchdog = new NodeWatchdog(component, leastUsedNode, timeoutCB);
#endif
        return leastUsedNode;

    } else {

        LOG_W("No available node right now.");
    }

    return NULL;
}

bool NodeManager::setIdle(long address) {

    TypeNodeList::iterator search = nodes.find(address);

    if (search == nodes.end()) {
        add(address);
        return false;
    }

    search->second->setState(IDLE);

    LOG_I("Node at address : %s switch to state : %s",
          InterfaceTypes::getAddressString(address).c_str(), sStates[IDLE]);

	return true;
}

bool NodeManager::validate(long address) {

    TypeNodeList::iterator search = nodes.find(address);
	if (search == nodes.end()) {
		add(address);
		return false;
	}

	LOG_I("Node at address : %s is Alive",
          InterfaceTypes::getAddressString(address).c_str());
	return true;

}

bool NodeManager::setBusy(long address) {

    TypeNodeList::iterator search = nodes.find(address);
    if (search == nodes.end()) {
        LOG_W("Could not found a node with address : %s",
              InterfaceTypes::getAddressString(address).c_str());
        return false;
    }


    NodeObject *node = search->second;

#ifndef DISABLE_RECOVERY
    if (node->watchdog != nullptr) {
        delete node->watchdog;
        node->watchdog = nullptr;
    }
#endif

    node->setState(BUSY);
    node->iterateUsage(true);

    LOG_T("Node at address : %s switch to state : %s",
          InterfaceTypes::getAddressString(address).c_str(), sStates[BUSY]);

	return true;

}

bool NodeManager::remove(long address) {

    TypeNodeList::iterator search = nodes.find(address);
    if (search == nodes.end()) {
        LOG_W("Could not found a node with address : %s",
              InterfaceTypes::getAddressString(address).c_str());
        return false;
    }

    NodeObject *node = search->second;
    nodes.erase(address);
    delete node;

#ifndef DISABLE_BACKUP
    readyBackup = (int) fmin(totalBackup, readyBackup + 1);

    LOG_U(UI_UPDATE_DIST_BACKUP, totalBackup, readyBackup);
    LOG_T("Node at address %s removed from the list",
          InterfaceTypes::getAddressString(address).c_str());
#endif

	return true;

}

bool NodeManager::add(long address) {

	nodes[address] = new NodeObject(address);

#ifndef DISABLE_BACKUP
	totalBackup = nodes.size() == 1 ? 0 : (uint32_t) ceil(nodes.size() * backupRate);

	readyBackup = 0;

	LOG_U(UI_UPDATE_DIST_BACKUP, totalBackup, readyBackup);
#endif

	LOG_I("Node at address : %s added to the list",
          InterfaceTypes::getAddressString(address).c_str());

	return true;
}

void NodeManager::clear() {

    for (TypeNodeList::iterator i = nodes.begin(); i != nodes.end(); i++) {
        NodeObject* node = i->second;
        delete node;
    }

    nodes.clear();

#ifndef DISABLE_BACKUP
	readyBackup = 0;
	totalBackup = 0;
#endif
}
