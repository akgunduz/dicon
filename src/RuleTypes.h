//
// Created by Haluk Akgunduz on 10/18/15.
//

#ifndef BANKOR_RULETYPES_H
#define BANKOR_RULETYPES_H

enum RULE_TYPES {
    RULE_RUNTYPE,
    RULE_FILES,
    RULE_PARAMETERS,
    RULE_EXECUTORS,
    RULE_MAX
};

class RuleTypes {

public:
    static const char* getName(RULE_TYPES id);
    static int getMax();
};


#endif //BANKOR_RULETYPES_H
