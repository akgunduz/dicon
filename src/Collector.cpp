//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Collector.h"
#include "Util.h"
#include "NetAddress.h"

Collector::Collector(const char *rootPath) :
        Component(Unit(COMP_COLLECTOR, Util::getArch()), rootPath){

	LOG_U(UI_UPDATE_COLL_ADDRESS, getAddress(COMP_DISTRIBUTOR), getAddress(COMP_NODE));

	distributorAddress = 0;

    jobs = new std::vector<Job*>();

    jobs->clear();
    std::vector<std::string> dirList = Util::getDirList(Unit::getRootPath(COMP_COLLECTOR), "Job_");
    for (int i = 0; i < dirList.size(); i++) {
        jobs->push_back(new Job(getInfo(), dirList[i].c_str()));
    }

    LOG_U(UI_UPDATE_COLL_JOB_LIST, jobs);
}

Collector::~Collector() {

	for (std::vector<Job*>::iterator i = jobs->begin(); i != jobs->end(); i++) {
        delete *i;
	}

    delete jobs;
}

bool Collector::processDistributorMsg(long address, Message *msg) {

	bool status = false;

	switch(msg->getType()) {

		case MSGTYPE_WAKEUP:

			distributorAddress = address;

			LOG_U(UI_UPDATE_COLL_ATT_DIST_ADDRESS, address);

			LOG_U(UI_UPDATE_COLL_LOG,
					"\"WAKEUP\" msg from distributor: %s", Address::getString(address).c_str());

			status = send2DistributorMsg(address, MSGTYPE_ALIVE);
			break;

		case MSGTYPE_NODE: {

            long nodeAddress = msg->getVariant(0);
			ARCH nodeArch = (ARCH) msg->getVariant(1);

			if (nodeAddress == 0) {
				LOG_W("No available node right now.");
				status = false;
				LOG_U(UI_UPDATE_COLL_LOG,
						"\"NODE\" msg from distributor: %s, no Available Node", Address::getString(address).c_str());
				break;
			}

            Job* job = getUnservedJob();
            if (job == nullptr) {
                break;
            }

            job->setAttachedNode(nodeAddress, nodeArch);
            FileList *list = job->prepareRuleList();

			LOG_T("New Job created from path : %s", "TODO JOB");

			LOG_U(UI_UPDATE_COLL_LOG,
                  "\"NODE\" msg from distributor: %s, available node: %s",
				  Address::getString(address).c_str(), Address::getString(nodeAddress).c_str());

            LOG_U(UI_UPDATE_COLL_PROCESS_LIST, job);

			status = send2NodeMsg(nodeAddress, MSGTYPE_RULE, list);
			break;
			}
		default :
			break;
	}

	delete msg;
	return status;
}

bool Collector::processCollectorMsg(long address, Message *msg) {
    return false;
}

bool Collector::processNodeMsg(long address, Message *msg) {

	bool status = false;

	switch(msg->getType()) {

		case MSGTYPE_MD5: {

            long jobID = msg->getVariant(0);

			LOG_U(UI_UPDATE_COLL_LOG,
					"\"MD5\" msg from node: %s with \"%d\" MD5 info",
				  Address::getString(address).c_str(), msg->md5List.size());

            Job* job = getJob(jobID);
            if (job == nullptr) {
                break;
            }
/*
			for (i = 0; i < msg->md5List.size(); i++) {
				//TODO contentler icinde ara
                for (int j = 0; j < job->getContentCount(CONTENT_FILE); j++) {
                    Rule *rule = (Rule *)job->getContent(CONTENT_FILE, j);
                    for (int k = 0; k < rule->getContentCount(CONTENT_FILE); k++) {
                        FileItem *content = (FileItem *)rule->getContent(CONTENT_FILE, k);
                        //TODO 64 bit le coz
                        if (memcmp(content->getMD5(), msg->md5List[i].md5, MD5_DIGEST_LENGTH) == 0) {
                            //TODO bu clientte var, gonderme
                            content->setFlaggedToSent(false);
                        }
                    }
                }
			}


*/

		//	LOG_U(UI_UPDATE_COLL_JOB_LIST, job);

            FileList *list = job->prepareFileList(Unit(COMP_COLLECTOR, job->getAttachedNode().getArch()));

            list->remove(&msg->md5List);

			status = send2NodeMsg(address, MSGTYPE_BINARY, list);
		}
			break;

		default:
			break;

	}

	delete msg;
	return status;
}

bool Collector::send2DistributorMsg(long address, int type) {

	Message *msg = new Message(COMP_COLLECTOR, type);

	switch(type) {

		case MSGTYPE_ALIVE:

			LOG_U(UI_UPDATE_COLL_LOG,
					"\"ALIVE\" msg sent to distributor: %s",
				  Address::getString(address).c_str());
			break;

		case MSGTYPE_NODE:

			LOG_U(UI_UPDATE_COLL_LOG,
					"\"NODE\" msg sent to distributor: %s",
				  Address::getString(address).c_str());
			break;

		case MSGTYPE_TIME:

			LOG_U(UI_UPDATE_COLL_LOG,
					"\"TIME\" msg sent to distributor: %s",
				  Address::getString(address).c_str());
			break;

		default:
			delete msg;
			return false;

	}

	return connectors[COMP_DISTRIBUTOR]->send(address, msg);

}

bool Collector::send2NodeMsg(long address, int type, FileList *list) {

	Message *msg = new Message(COMP_COLLECTOR, type);

    msg->setVariant(0, list->getID());

	switch(type) {

		case MSGTYPE_RULE:

			msg->setJob(STREAM_JOB, list);
			LOG_U(UI_UPDATE_COLL_LOG,
					"\"RULE\" msg sent to node: %s",
				  Address::getString(address).c_str());
			break;

		case MSGTYPE_BINARY:

			msg->setJob(STREAM_BINARY, list);
			LOG_U(UI_UPDATE_COLL_LOG,
					"\"BINARY\" msg sent to node: %s with \"%d\" file binary",
				  Address::getString(address).c_str(), list->getCount());
			break;

		default:
			delete msg;
			return false;

	}

	return connectors[COMP_NODE]->send(address, msg);

}

bool Collector::processRule() {

    for (int i = 0; i < jobs->size(); i++) {
        send2DistributorMsg(distributorAddress, MSGTYPE_NODE);
    }
    return true;
}

bool Collector::syncTime() {

	return send2DistributorMsg(distributorAddress, MSGTYPE_TIME);

}

bool Collector::reset() {

	for (int i = 0; i < jobs->size(); i++) {
        (*jobs)[i]->setAttachedNode(0, ARCH_MAX);
    }
	return true;

}

Job* Collector::getJob(long id) {

    int i = 0;
    for (; i < jobs->size(); i++) {
        if ((*jobs)[i]->getID() != id) {
            continue;
        }
        break;
    }

    if (i == jobs->size()) {
        LOG_W("No job found with id %ld", id);
        return nullptr;
    }

    return (*jobs)[i];
}

Job* Collector::getAttachedJob(long address) {

    int i = 0;
    for (; i < jobs->size(); i++) {
        if ((*jobs)[i]->getAttachedNode().address != address) {
            continue;
        }
        break;
    }

    if (i == jobs->size()) {
        LOG_W("No job found attached to %ld", address);
        return nullptr;
    }

    return (*jobs)[i];
}

Job* Collector::getUnservedJob() {

    int i = 0;
    for (; i < jobs->size(); i++) {
        if ((*jobs)[i]->getAttachedNode().address > 0) {
            continue;
        }
        break;
    }

    if (i == jobs->size()) {
        LOG_W("No unserved job remains.");
        return nullptr;
    }

    return (*jobs)[i];
}
