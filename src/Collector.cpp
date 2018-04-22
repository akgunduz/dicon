//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Collector.h"

Collector::Collector(const char *rootPath) :
        Component(COMP_COLLECTOR, rootPath){

    processMsg[COMP_DISTRIBUTOR][MSGTYPE_WAKEUP] = static_cast<TypeProcessComponentMsg>(&Collector::processDistributorWakeupMsg);
    processMsg[COMP_DISTRIBUTOR][MSGTYPE_NODE] = static_cast<TypeProcessComponentMsg>(&Collector::processDistributorNodeMsg);
    processMsg[COMP_NODE][MSGTYPE_INFO] = static_cast<TypeProcessComponentMsg>(&Collector::processNodeInfoMsg);

	LOG_U(UI_UPDATE_COLL_ADDRESS, getInterfaceAddress(COMP_DISTRIBUTOR), getInterfaceAddress(COMP_NODE));

    setDistributorAddress(0);

    getJobs()->addList(getHost(), ComponentTypes::getRootPath(COMP_COLLECTOR), true);

    LOG_U(UI_UPDATE_COLL_FILE_LIST, getJobs()->get(0));
	LOG_U(UI_UPDATE_COLL_PROCESS_LIST, getJobs()->get(0));
}

Collector::~Collector() {

}

bool Collector::processDistributorWakeupMsg(long address, Message *msg) {

    setDistributorAddress(address);

    LOG_U(UI_UPDATE_COLL_ATT_DIST_ADDRESS, address);

    return send2DistributorMsg(address, MSGTYPE_ALIVE, NULL);
}

bool Collector::processDistributorNodeMsg(long address, Message *msg) {

    long nodeAddress = msg->getHeader()->getVariant(0);

    if (nodeAddress == 0) {

        LOG_U(UI_UPDATE_COLL_LOG, "No Available Node");
        delete msg;
        return false;
    }

    //TODO
    ExecutorItem* item = getJobs()->get(0)->getUnServed();

    if (item == NULL) {
        LOG_W("No available unServed job right now.");
        delete msg;
        return false;
    }

    getJobs()->get(0)->attachNode(item, nodeAddress);

    //  FileList *list = getJobs()->getJob(0)->prepareRuleList();

    LOG_T("New Job created from path : %s", "TODO JOB");

    LOG_U(UI_UPDATE_COLL_LOG, "Available Node : %s",
          InterfaceTypes::getAddressString(nodeAddress).c_str());

    //   LOG_U(UI_UPDATE_COLL_PROCESS_LIST, job);

//            TypeMD5List md5List;
//            for (int i = 0; i < item->getDependentFileCount(); i++) {
//                md5List.push_back(*item->getDependentFile(i)->getMD5());
//            }

    return send2NodeMsg(nodeAddress, MSGTYPE_JOB, getJobs()->get(0)->getJobDir(),
                          item->getParsedExec(), item->getDependentFileList());
}

bool Collector::processNodeInfoMsg(long address, Message *msg) {

    LOG_U(UI_UPDATE_COLL_LOG, "%d File info received", msg->getData()->getFileCount());

    return send2NodeMsg(address, MSGTYPE_BINARY, msg->getData()->getFileList());
}

bool Collector::send2DistributorMsg(long address, MSG_TYPE type, ...) {

    va_list ap;
    va_start(ap, type);

	Message *msg = new Message(COMP_COLLECTOR, type);

	switch(type) {

		case MSGTYPE_ALIVE:
			break;

        case MSGTYPE_NODE: {
			TypeMD5List *md5List = va_arg(ap, TypeMD5List*);
			msg->getData()->setStreamFlag(STREAM_MD5);
			msg->getData()->addMD5List(md5List);
			LOG_U(UI_UPDATE_COLL_LOG, "\"%d\" necessary file md5s are prepared", md5List->size());
		}
            break;

        default:
			delete msg;
			va_end(ap);
			return false;

	}

	va_end(ap);

	return send(COMP_DISTRIBUTOR, address, msg);

}

bool Collector::send2NodeMsg(long address, MSG_TYPE type, ...) {

    va_list ap;
    va_start(ap, type);

	Message *msg = new Message(COMP_COLLECTOR, type);

	switch(type) {

		case MSGTYPE_JOB: {
            char *jobDir = va_arg(ap, char*);
            char *executor = va_arg(ap, char*);
            TypeFileList *fileList = va_arg(ap, TypeFileList*);
            msg->getData()->setStreamFlag(STREAM_JOB);
            msg->getData()->setJobDir(jobDir);
            msg->getData()->setExecutor(executor);
            msg->getData()->addFileList(fileList);
            LOG_U(UI_UPDATE_COLL_LOG, "\"%d\" file info is prepared for execution : %s", fileList->size(), executor);
        }
			break;

		case MSGTYPE_BINARY: {
            TypeFileList *fileList = va_arg(ap, TypeFileList*);
            msg->getData()->setStreamFlag(STREAM_BINARY);
            msg->getData()->addFileList(fileList);
            LOG_U(UI_UPDATE_COLL_LOG, "\"%d\" file binary is prepared", fileList->size());
        }
			break;

		default:
			delete msg;
            va_end(ap);
			return false;

	}

    va_end(ap);

	return send(COMP_NODE, address, msg);
}

bool Collector::processJob() {

    //TODO Whole executors will be replaced with only independent executors
    //TODO Also will add other jobs, after the prev. job is done.


    for (int k = 0; k < jobs.get(0)->getContentCount(CONTENT_EXECUTOR); k++) {

        TypeMD5List md5List;
        ExecutorItem *executorItem = (ExecutorItem *)jobs.get(0)->getContent(CONTENT_EXECUTOR, k);
        for (int i = 0; i < executorItem->getDependentFileCount(); i++) {
            md5List.push_back(*executorItem->getDependentFile(i)->getMD5());
        }

        send2DistributorMsg(distributorAddress, MSGTYPE_NODE, &md5List);
    }

    return true;
}

Jobs *Collector::getJobs() {

    return &jobs;
}

bool Collector::setDistributorAddress(long address) {

    distributorAddress = address;
    return true;
}
