//
// Created by akgunduz on 23.10.2015.
//

#include "SchedulerItem.h"

SchedulerItem::SchedulerItem(int _type, int _priority)
    : type(_type), priority(_priority) {

}

SchedulerItem::SchedulerItem(int _type)
    : type(_type), priority(DEFAULT_PRIORITY) {

}

SchedulerItem::SchedulerItem()
    : type(END_ITEM), priority(WORST_PRIORITY) {

}

SchedulerItem::~SchedulerItem() = default;