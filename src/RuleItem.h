//
// Created by akgunduz on 26.10.2015.
//

#ifndef BANKOR_RULEITEM_H
#define BANKOR_RULEITEM_H

#include "Common.h"
#include "SchedulerItem.h"

#define RULE_ITEM 1

class RuleItem : public SchedulerItem {

    char path[PATH_MAX];
    bool active;
    int repeat;

public:
    RuleItem(const char*, bool, int);
};


#endif //BANKOR_RULEITEM_H
