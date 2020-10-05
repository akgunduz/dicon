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

    static constexpr const char* sComponentTypes[COMP_MAX] = {
            "Distributor",
            "Collector",
            "Node",
    };

public:

    static const char* getName(COMPONENT _type) {

        return sComponentTypes[_type];
    }

};

#endif //DICON_COMPONENTTYPE_H
