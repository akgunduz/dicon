//
// Created by akgunduz on 23.10.2015.
//

#ifndef DICON_SCHEDULERITEM_H
#define DICON_SCHEDULERITEM_H

#include "Common.h"

#define END_ITEM -1
#define DEFAULT_PRIORITY 3
#define WORST_PRIORITY 100

class SchedulerItem {
public:
    int type;
    int priority;
    SchedulerItem(int, int);
    SchedulerItem(int);
    SchedulerItem();
    virtual ~SchedulerItem();
};

typedef std::shared_ptr<SchedulerItem> TypeSchedulerItem;

#endif //DICON_SCHEDULERITEM_H
