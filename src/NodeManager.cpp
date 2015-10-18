//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "NodeManager.h"

NodeManager::NodeManager(Component* component, fTimeoutCB timeoutCB, fWakeupCB wakeupCB, double backupRate) {

    this->component = component;
	this->timeoutCB = timeoutCB;
	this->wakeupCB = wakeupCB;
	this->backupRate = backupRate;

    if (nodeWatchdog == nullptr) {
        nodeWatchdog = new NodeWatchdog(component, wakeupCB);
    }
};

NodeManager::~NodeManager() {

    if (nodeWatchdog != nullptr) {
        delete nodeWatchdog;
    }

	for (std::map<long, NodeItem *>::iterator i = nodes.begin(); i != nodes.end(); i++) {
		NodeItem* node = i->second;
		delete node;
	}
}

NodeItem *NodeManager::getIdle(long collectorAddress) {

//TODO coklu collector de burada thread korumasi olmasi lazim

    NodeItem *leastUsedNode = nullptr;

    int idleCount = 0;

    for (std::map<long, NodeItem *>::iterator i = nodes.begin(); i != nodes.end(); i++) {
        NodeItem *node = i->second;
        if (node->state != IDLE) {
            continue;
        }

        idleCount++;

        if (leastUsedNode == nullptr) {
            leastUsedNode = node;
            continue;
        }

        if (node->usage < leastUsedNode->usage) {
            leastUsedNode = node;
        }
    }

    if (leastUsedNode != nullptr && idleCount - totalBackup + readyBackup > 0) {
        leastUsedNode->state = PREBUSY;
        leastUsedNode->lastServedCollector = collectorAddress;

        if (leastUsedNode->watchdog != nullptr) {
            delete leastUsedNode->watchdog;
            leastUsedNode->watchdog = nullptr;
        }
        leastUsedNode->watchdog = new NodeWatchdog(component, leastUsedNode, timeoutCB);

        return leastUsedNode;

    } else {

        LOG_W("No available client right now.");
    }

    return nullptr;
}

bool NodeManager::setIdle(long address, short id, double totalTime) {

	try {

		NodeItem *node = nodes.at(address);

        node->state = IDLE;
		LOG_T("Node at address : %s switch to state : %s", Address::getString(address).c_str(), sStates[IDLE]);

		if (node->stopWatch.isInitiated()) {
			LOG_U(UI_UPDATE_DIST_LOG,
					"Node at address : %s finished job in %.3lf seconds, total time passed : %.3lf",
				  Address::getString(address).c_str(), node->stopWatch.stop(), totalTime);
		}

	} catch (const std::out_of_range e) {

		add(address, id);
		return false;
	}

	return true;

}

bool NodeManager::validate(long address, short id) {

	std::map<long, NodeItem *>::iterator i = nodes.find(address);
	if (i == nodes.end()) {
		add(address, id);
		return false;
	}

	LOG_T("Node at address : %s is Alive", Address::getString(address).c_str());
	return true;

}

bool NodeManager::setBusy(long address) {

	try {

		NodeItem *node = nodes.at(address);
        if (node->watchdog != nullptr) {
            delete node->watchdog;
            node->watchdog = nullptr;
        }

        node->state = BUSY;
        node->usage++;
        node->stopWatch.start();
		LOG_T("Node at address : %s switch to state : %s", Address::getString(address).c_str(), sStates[BUSY]);

	} catch (const std::out_of_range e) {
		LOG_E("Could not found a client with address : %s", Address::getString(address).c_str());
		return false;
	}

	return true;

}

bool NodeManager::remove(long address) {

	try {

		NodeItem *node = nodes.at(address);
		nodes.erase(address);
		delete node;

		readyBackup = (int) fmin(totalBackup, readyBackup + 1);

		LOG_U(UI_UPDATE_DIST_BACKUP, totalBackup, readyBackup);
		LOG_T("Node at address %s removed from the list", Address::getString(address).c_str());

	} catch (const std::out_of_range e) {
		LOG_E("Could not found a client with address : %s", Address::getString(address).c_str());
		return false;
	}

	return true;

}

bool NodeManager::add(long address, short id) {

	nodes[address] = new NodeItem(IDLE, 0, address, id);

	totalBackup = nodes.size() == 1 ? 0 : (uint32_t) ceil(nodes.size() * backupRate);

	readyBackup = 0;

	LOG_U(UI_UPDATE_DIST_BACKUP, totalBackup, readyBackup);
	LOG_T("Node at address : %s added to the list", Address::getString(address).c_str());

	return true;
}



bool NodeManager::resetTimes() {

	for (std::map<long, NodeItem *>::iterator i = nodes.begin(); i != nodes.end(); i++) {
		i->second->stopWatch.reset();
	}
	return true;
}

void NodeManager::clear() {

    for (std::map<long, NodeItem *>::iterator i = nodes.begin(); i != nodes.end(); i++) {
        NodeItem* node = i->second;
        delete node;
    }

    nodes.clear();
	readyBackup = 0;
	totalBackup = 0;
}
