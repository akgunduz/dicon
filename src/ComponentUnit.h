//
// Created by akgun on 7.07.2020.
//

#ifndef DICON_COMPONENTUNIT_H
#define DICON_COMPONENTUNIT_H

#include "Address.h"
#include "ComponentType.h"

class ComponentUnit {

    uint32_t id{};

    uint16_t type{COMP_MAX};

    Address address{};

    friend class MessageHeader;

public:

    ComponentUnit() = default;
    explicit ComponentUnit(COMPONENT);
    ComponentUnit(COMPONENT, long, Address);
    ComponentUnit(const ComponentUnit&);

    long getID() const;
    void setID(long);

    COMPONENT getType() const;
    void setType(COMPONENT);

    Address& getAddress();
    void setAddress(Address, bool = false);

    void grab(ComponentUnit&);

    static COMPONENT next(COMPONENT);
    static COMPONENT prev(COMPONENT);
};

typedef std::vector<ComponentUnit> TypeComponentList;


#endif //DICON_COMPONENTUNIT_H
