//
// Created by akgun on 7.07.2020.
//

#ifndef DICON_COMPONENTUNIT_H
#define DICON_COMPONENTUNIT_H

#include "CommUnit.h"

typedef std::chrono::steady_clock::time_point TypeTime;

class ComponentUnit : public CommUnit {

    void* handle{};
    TypeTime checkTime{};

public:

    ComponentUnit() = default;
    ComponentUnit(COMPONENT, ARCH, TypeID, Address);
    ComponentUnit(COMPONENT, Address);
    explicit ComponentUnit(COMPONENT);
    ComponentUnit(const ComponentUnit&);
    explicit ComponentUnit(const TypeCommUnit&);
    explicit ComponentUnit(const CommUnit*);
    virtual ~ComponentUnit();

    void* getHandle() const;
    void setHandle(void*);

    TypeTime& getCheckTime();
    void setCheckTime(TypeTime);
};

typedef std::shared_ptr<ComponentUnit> TypeComponentUnit;
typedef std::vector<TypeComponentUnit> TypeComponentUnitList;
typedef std::map<TypeID, TypeComponentUnit> TypeComponentMapIDList;
typedef std::map<TypeID, TypeComponentUnitList> TypeComponentListMapIDList;

#endif //DICON_COMPONENTUNIT_H
