//
// Created by akgun on 7.07.2020.
//

#ifndef DICON_COMPONENTUNIT_H
#define DICON_COMPONENTUNIT_H

#include "CommUnit.h"

class ComponentUnit : public CommUnit {

    int socket{};

public:

    ComponentUnit() = default;
    explicit ComponentUnit(COMPONENT);
    ComponentUnit(COMPONENT, long, Address);
    ComponentUnit(const ComponentUnit&);
    ComponentUnit(const CommUnit&);

    int getSocket();
    void setSocket(int);
};

typedef std::vector<ComponentUnit> TypeComponentList;

#endif //DICON_COMPONENTUNIT_H
