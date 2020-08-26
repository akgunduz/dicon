//
// Created by akgun on 7.07.2020.
//

#ifndef DICON_COMPONENTUNIT_H
#define DICON_COMPONENTUNIT_H

#include "CommUnit.h"

class ComponentUnit : public CommUnit {

    int socket{};
    long long checkTime{};

public:

    ComponentUnit() = default;
    ComponentUnit(COMPONENT, ARCH, long, Address);
    ComponentUnit(COMPONENT, Address);
    ComponentUnit(const ComponentUnit&);
    explicit ComponentUnit(const TypeCommUnit&);
    explicit ComponentUnit(int);
    virtual ~ComponentUnit();

    int getSocket() const;
    void setSocket(int);

    long getCheckTime() const;
    void setCheckTime(long checkTime);
};

typedef std::shared_ptr<ComponentUnit> TypeComponentUnit;
typedef std::vector<TypeComponentUnit> TypeComponentUnitList;
typedef std::map<long, TypeComponentUnit> TypeComponentMapIDList;

#endif //DICON_COMPONENTUNIT_H
