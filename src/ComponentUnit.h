//
// Created by akgun on 7.07.2020.
//

#ifndef DICON_COMPONENTUNIT_H
#define DICON_COMPONENTUNIT_H

#include "CommUnit.h"

class ComponentUnit : public CommUnit {

    int socket{};
    long checkTime{};

public:

    ComponentUnit() = default;
    explicit ComponentUnit(COMPONENT);
    ComponentUnit(COMPONENT, ARCH, long, Address);
    ComponentUnit(const ComponentUnit&);
    ComponentUnit(const CommUnit&);

    int getSocket() const;
    void setSocket(int);

    long getCheckTime() const;
    void setCheckTime(long checkTime);
};

typedef std::vector<ComponentUnit*> TypeComponentList;

#endif //DICON_COMPONENTUNIT_H
