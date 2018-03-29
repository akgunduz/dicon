//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Collector.h"

Collector::Collector(const char *rootPath) :
        Component(Unit(COMP_COLLECTOR), rootPath){

	LOG_U(UI_UPDATE_COLL_ADDRESS, getAddress(COMP_DISTRIBUTOR), getAddress(COMP_NODE));

    setDistributorAddress(0);

    getJobs()->addJobList(getHost(), Unit::getRootPath(COMP_COLLECTOR), true);

    LOG_U(UI_UPDATE_COLL_JOB_LIST, getJobs());
}

Collector::~Collector() {

}

bool Collector::processDistributorMsg(long address, Message *msg) {

	bool status = false;

	switch(msg->getHeader()->getType()) {

		case MSGTYPE_WAKEUP:

			setDistributorAddress(address);

			LOG_U(UI_UPDATE_COLL_ATT_DIST_ADDRESS, address);

			status = send2DistributorMsg(address, MSGTYPE_ALIVE);
			break;

		case MSGTYPE_NODE: {

            NodeInfo node(msg->getHeader()->getVariant(0), (ARCH) msg->getHeader()->getVariant(1));

			if (node.getAddress() == 0) {

				status = false;
				LOG_U(UI_UPDATE_COLL_LOG, "No Available Node");
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

            LOG_U(UI_UPDATE_COLL_LOG, "Available Node : %s", Address::getString(node.getAddress()).c_str());

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

	switch(msg->getHeader()->getType()) {

		case MSGTYPE_MD5: {

			LOG_U(UI_UPDATE_COLL_LOG, "MD5 info size %d", msg->md5List.size());

            NodeInfo node(address, msg->getHeader()->getOwner().getArch());

            Job* job = getJobs()->getJobByNode(node);
            if (job == NULL) {
                break;
            }

			//LOG_U(UI_UPDATE_COLL_JOB_LIST, job);

            FileList *list = job->prepareFileList(node.getArch());

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

bool Collector::send2DistributorMsg(long address, MSG_TYPE type) {

	Message *msg = new Message(COMP_COLLECTOR, type);

	switch(type) {

		case MSGTYPE_ALIVE:
		case MSGTYPE_NODE:
		case MSGTYPE_TIME:
			break;

		default:
			delete msg;
			return false;

	}

	return send(COMP_DISTRIBUTOR, address, msg);

}

bool Collector::send2NodeMsg(long address, MSG_TYPE type, FileList *list) {

	Message *msg = new Message(COMP_COLLECTOR, type);

	switch(type) {

		case MSGTYPE_RULE:
			msg->setJob(STREAM_JOB, list);
			break;

		case MSGTYPE_BINARY:
			msg->setJob(STREAM_BINARY, list);
            LOG_U(UI_UPDATE_COLL_LOG, "\"%d\" file binary is prepared", list->getCount());
			break;

		default:
			delete msg;
			return false;

	}

	return send(COMP_NODE, address, msg);

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
