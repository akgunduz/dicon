//
// Created by akgunduz on 26.10.2015.
//

#include "RuleItem.h"

RuleItem::RuleItem(const char *path, bool active = true, int repeat = 1)
        : SchedulerItem(RULE_ITEM) {

    strcpy(this->path, path);
    this->active = active;
    this->repeat = repeat;
}
