//
// Created by akgun on 7.07.2020.
//

#ifndef DICON_COMPONENTUNIT_H
#define DICON_COMPONENTUNIT_H

#include <uv/include/uv.h>
#include "CommUnit.h"

class ComponentUnit : public CommUnit {

    uv_stream_t* handle{};
    long long checkTime{};

public:

    ComponentUnit() = default;
    ComponentUnit(COMPONENT, ARCH, TypeID, Address);
    ComponentUnit(COMPONENT, Address);
    explicit ComponentUnit(COMPONENT);
    ComponentUnit(const ComponentUnit&);
    explicit ComponentUnit(const TypeCommUnit&);
    virtual ~ComponentUnit();

    uv_stream_t* getHandle() const;
    void setHandle(uv_stream_t*);

    long long getCheckTime() const;
    void setCheckTime(long long checkTime);
};

typedef std::shared_ptr<ComponentUnit> TypeComponentUnit;
typedef std::vector<TypeComponentUnit> TypeComponentUnitList;
typedef std::map<TypeID, TypeComponentUnit> TypeComponentMapIDList;
typedef std::map<TypeID, TypeComponentUnitList> TypeComponentListMapIDList;

#endif //DICON_COMPONENTUNIT_H
