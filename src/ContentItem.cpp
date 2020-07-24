//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "ContentItem.h"

ContentItem::ContentItem(const ComponentObject& _host, long _id, long _assignedJob) :
        host(_host), id(_id), assignedJob(_assignedJob) {
}

ContentItem::ContentItem(const ContentItem &copy) :
        SchedulerItem(copy),
        host(copy.getHost()),
        id(copy.getID()),
        assignedJob(copy.getAssignedJob()){
}

const ComponentObject& ContentItem::getHost() const {

    return host;
}

long ContentItem::getID() const {
    return id;
}

void ContentItem::setID(long _id) {
    id = _id;
}

long ContentItem::getAssignedJob() const {

    return assignedJob;
}

void ContentItem::setAssignedJob(long _assignedJob) {

    assignedJob = _assignedJob;
}
