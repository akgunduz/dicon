//
// Created by Haluk AKGUNDUZ on 1.08.2020.
//

#ifndef DICON_COMPONENTTYPE_H
#define DICON_COMPONENTTYPE_H

enum COMPONENT {

    COMP_DISTRIBUTOR,
    COMP_COLLECTOR,
    COMP_NODE,
    COMP_MAX
};

class ComponentType {

public:
    static const char* getName(COMPONENT);

};


#endif //DICON_COMPONENTTYPE_H
