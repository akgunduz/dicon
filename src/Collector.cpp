//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Collector.h"

Collector::Collector(const char *rootPath) :
        Component(Unit(COMP_COLLECTOR, Util::getArch()), rootPath){

	LOG_U(UI_UPDATE_COLL_ADDRESS, getAddress(COMP_DISTRIBUTOR), getAddress(COMP_NODE));

    setDistributorAddress(0);

    getJobs()->addJobList(getHost(), Unit::getRootPath(COMP_COLLECTOR), true);

    LOG_U(UI_UPDATE_COLL_JOB_LIST, getJobs());
}

Collector::~Collector() {

}

bool Collector::processDistributorMsg(long address, Message *msg) {

	bool status = false;

	switch(msg->getType()) {

		case MSGTYPE_WAKEUP:

			setDistributorAddress(address);

			LOG_U(UI_UPDATE_COLL_ATT_DIST_ADDRESS, address);

			LOG_U(UI_UPDATE_COLL_LOG,
					"\"WAKEUP\" msg from distributor: %s", Address::getString(address).c_str());

			status = send2DistributorMsg(address, MSGTYPE_ALIVE);
			break;

		case MSGTYPE_NODE: {

            NodeInfo node(msg->getVariant(0), (ARCH) msg->getVariant(1));

			if (node.getAddress() == 0) {
				LOG_W("No available node right now.");
				status = false;
				LOG_U(UI_UPDATE_COLL_LOG,
						"\"NODE\" msg from distributor: %s, no Available Node", Address::getString(address).c_str());
				break;
			}

            Job* job = getJobs()->getJobUnServed();

            if (job == nullptr) {
                LOG_W("No available unServed job right now.");
                break;
            }

            jobs.attachNode(job, node);

            FileList *list = job->prepareRuleList();

			LOG_T("New Job created from path : %s", "TODO JOB");

			LOG_U(UI_UPDATE_COLL_LOG,
                  "\"NODE\" msg from distributor: %s, available node: %s",
				  Address::getString(address).c_str(), Address::getString(node.getAddress()).c_str());

            LOG_U(UI_UPDATE_COLL_PROCESS_LIST, job);

			status = send2NodeMsg(node.getAddress(), MSGTYPE_RULE, list);
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

			LOG_U(UI_UPDATE_COLL_LOG,
					"\"MD5\" msg from node: %s with \"%d\" MD5 info",
				  Address::getString(address).c_str(), msg->md5List.size());

            NodeInfo node = getJobs()->getNodeByAddress(address);

            Job* job = getJobs()->getJobByNode(node);
            if (job == nullptr) {
                break;
            }

			//LOG_U(UI_UPDATE_COLL_JOB_LIST, job);

            FileList *list = job->prepareFileList(Unit(COMP_COLLECTOR, node.getArch()));

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

    for (int i = 0; i < getJobs()->getCount(); i++) {
        send2DistributorMsg(distributorAddress, MSGTYPE_NODE);
    }
    return true;
}

bool Collector::syncTime() {

	return send2DistributorMsg(distributorAddress, MSGTYPE_TIME);

}

Jobs *Collector::getJobs() {

    return &jobs;
}

bool Collector::setDistributorAddress(long address) {

    distributorAddress = address;
    return true;
}
