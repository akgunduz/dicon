//
// Created by akgun on 7.07.2020.
//

#ifndef DICON_COMPONENTUNIT_H
#define DICON_COMPONENTUNIT_H

#include "CommUnit.h"
#include "UtilTime.h"

class ComponentUnit : public CommUnit {

    void* handle{};
    UtilTime timer{};

public:

    ComponentUnit() = default;
    ComponentUnit(COMPONENT, ARCH, TypeID, TypeAddress);
    ComponentUnit(COMPONENT, TypeAddress);
    explicit ComponentUnit(COMPONENT);
    ComponentUnit(const ComponentUnit&);
    explicit ComponentUnit(const TypeCommUnit&);
    explicit ComponentUnit(const CommUnit*);
    explicit ComponentUnit(const BaseUnit*);
    virtual ~ComponentUnit();

    void* getHandle() const;
    void setHandle(void*);

    TypeTimeDuration getDuration();
    void resetTimer();
};

typedef std::shared_ptr<ComponentUnit> TypeComponentUnit;
typedef std::vector<TypeComponentUnit> TypeComponentUnitList;
typedef std::map<TypeID, TypeComponentUnit> TypeComponentMapIDList;
typedef std::map<TypeID, TypeComponentUnitList> TypeComponentListMapIDList;

#endif //DICON_COMPONENTUNIT_H
