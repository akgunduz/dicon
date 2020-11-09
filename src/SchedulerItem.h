//
// Created by Haluk AKGUNDUZ on 23.10.2015.
// Copyright (c) 2015 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_SCHEDULERITEM_H
#define DICON_SCHEDULERITEM_H

#include <memory>

#define END_ITEM -1
#define DEFAULT_PRIORITY 3
#define WORST_PRIORITY 100

class SchedulerItem {
public:
    int type;
    int priority;
    SchedulerItem(int, int);
    explicit SchedulerItem(int);
    SchedulerItem();
    virtual ~SchedulerItem();
};

typedef std::shared_ptr<SchedulerItem> TypeSchedulerItem;
typedef bool (*TypeSchedulerCB)(void*, const TypeSchedulerItem&);

#endif //DICON_SCHEDULERITEM_H
