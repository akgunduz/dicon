//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Collector.h"

Collector::Collector(const char *rootPath) :
        Component(COMP_COLLECTOR, rootPath){

	LOG_U(UI_UPDATE_COLL_ADDRESS, getInterfaceAddress(COMP_DISTRIBUTOR), getInterfaceAddress(COMP_NODE));

    setDistributorAddress(0);

    getJobs()->addList(getHost(), ComponentTypes::getRootPath(COMP_COLLECTOR), true);

    LOG_U(UI_UPDATE_COLL_FILE_LIST, getJobs()->get(0));
	LOG_U(UI_UPDATE_COLL_PROCESS_LIST, getJobs()->get(0));
}

Collector::~Collector() {

}

bool Collector::processDistributorMsg(long address, Message *msg) {

	bool status = false;

	switch(msg->getHeader()->getType()) {

		case MSGTYPE_WAKEUP:

			setDistributorAddress(address);

			LOG_U(UI_UPDATE_COLL_ATT_DIST_ADDRESS, address);

			status = send2DistributorMsg(address, MSGTYPE_ALIVE, NULL);
			break;

		case MSGTYPE_NODE: {

            long nodeAddress = msg->getHeader()->getVariant(0);

			if (nodeAddress == 0) {

				status = false;
				LOG_U(UI_UPDATE_COLL_LOG, "No Available Node");
				break;
			}

			//TODO
            ExecutorItem* item = getJobs()->get(0)->getUnServed();

            if (item == NULL) {
                LOG_W("No available unServed job right now.");
                break;
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

            status = send2NodeMsg(nodeAddress, MSGTYPE_JOB, getJobs()->get(0)->getJobDir(),
                                  item->getParsedExec(), item->getDependentFileList());
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

		case MSGTYPE_INFO: {

			LOG_U(UI_UPDATE_COLL_LOG, "%d File info received", msg->getData()->getFileCount());

            status = send2NodeMsg(address, MSGTYPE_BINARY, msg->getData()->getFileList());

		}
			break;

		default:
			break;

	}

	delete msg;
	return status;
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
			LOG_U(UI_UPDATE_COLL_LOG, "\"%d\" file md5 is prepared", md5List->size());
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


  //  for (int i = 0; i < getJobs()->getJob(0)->getCount(); i++) {
	TypeMD5List md5List;
	ExecutorItem *executorItem = (ExecutorItem *)jobs.get(0)->getContent(CONTENT_EXECUTOR, 0);
	for (int i = 0; i < executorItem->getDependentFileCount(); i++) {
		md5List.push_back(*executorItem->getDependentFile(i)->getMD5());
	}

	send2DistributorMsg(distributorAddress, MSGTYPE_NODE, &md5List);
  //  }

    return true;
}

Jobs *Collector::getJobs() {

    return &jobs;
}

bool Collector::setDistributorAddress(long address) {

    distributorAddress = address;
    return true;
}
