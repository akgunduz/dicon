//
// Created by Haluk Akgunduz on 10/18/15.
//

#include "RuleTypes.h"

const char* sRuleTypes[] = {
        "runtype",
        "files",
        "parameters",
        "executors",
};

const char *RuleTypes::getName(RULE_TYPES id) {
    return sRuleTypes[id];
}

int RuleTypes::getMax() {
    return RULE_MAX;
}