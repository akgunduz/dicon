//
// Created by Haluk AKGUNDUZ on 29/05/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Collector.h"

Collector::Collector(const char *rootPath) :
        Component(Unit(COMP_COLLECTOR), rootPath){

	LOG_U(UI_UPDATE_COLL_ADDRESS, getInterfaceAddress(COMP_DISTRIBUTOR), getInterfaceAddress(COMP_NODE));

    setDistributorAddress(0);

    getJobs()->addJobList(getHost(), Unit::getRootPath(COMP_COLLECTOR), true);

    LOG_U(UI_UPDATE_COLL_FILE_LIST, getJobs()->getJob(0));
	LOG_U(UI_UPDATE_COLL_PROCESS_LIST, getJobs()->getJob(0));
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

            NodeInfo node(msg->getHeader()->getVariant(0), (ARCH) msg->getHeader()->getVariant(1));

			if (node.getAddress() == 0) {

				status = false;
				LOG_U(UI_UPDATE_COLL_LOG, "No Available Node");
				break;
			}

			//TODO
            ExecutorItem* item = getJobs()->getJob(0)->getUnServed();

            if (item == NULL) {
                LOG_W("No available unServed job right now.");
                break;
            }

            getJobs()->getJob(0)->attachNode(item, node);

          //  FileList *list = getJobs()->getJob(0)->prepareRuleList();

			LOG_T("New Job created from path : %s", "TODO JOB");

            LOG_U(UI_UPDATE_COLL_LOG, "Available Node : %s",
				  InterfaceTypes::getAddressString(node.getAddress()).c_str());

         //   LOG_U(UI_UPDATE_COLL_PROCESS_LIST, job);

			status = send2NodeMsg(node.getAddress(), MSGTYPE_JOB, item);
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

			LOG_U(UI_UPDATE_COLL_LOG, "MD5 info size %d", msg->getData()->getMD5Count());

            NodeInfo node(address, msg->getHeader()->getOwner().getArch());


            //TODO
//            Job* job = getJobs()->getJobByNode(node);
//            if (job == NULL) {
//                break;
//            }

			//LOG_U(UI_UPDATE_COLL_FILE_LIST, job);

//            FileList *list = job->prepareFileList(node.getArch());
//
//            list->remove(&msg->md5List);
//
//			status = send2NodeMsg(address, MSGTYPE_BINARY, list);
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
			msg->getData()->setStreamFlag(STREAM_MD5ONLY);
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
            char *executor = va_arg(ap, char*);
            TypeMD5List *md5List = va_arg(ap, TypeMD5List*);
            msg->getData()->setStreamFlag(STREAM_JOB);
            msg->getData()->setExecutor(executor);
            msg->getData()->addMD5List(md5List);
            LOG_U(UI_UPDATE_COLL_LOG, "\"%d\" file md5 is prepared for execution : %s", md5List->size(), executor);
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
	ExecutorItem *executorItem = (ExecutorItem *)jobs.getJob(0)->getContent(CONTENT_EXECUTOR, 0);
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
