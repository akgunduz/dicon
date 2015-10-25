//
// Created by akgunduz on 23.10.2015.
//

#ifndef BANKOR_SCHEDULERITEM_H
#define BANKOR_SCHEDULERITEM_H

#define END_ITEM -1
#define DEFAULT_PRIORITY 3
#define WORST_PRIORITY 100

class SchedulerItem {
public:
    int type;
    int priority;
    SchedulerItem(int _type, int _priority) : type(_type), priority(_priority) {}
    SchedulerItem(int _type) : type(_type), priority(DEFAULT_PRIORITY) {}
    SchedulerItem() : type(END_ITEM), priority(WORST_PRIORITY) {}
};


#endif //BANKOR_SCHEDULERITEM_H
