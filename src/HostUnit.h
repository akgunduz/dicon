//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#ifndef DICON_HOSTUNIT_H
#define DICON_HOSTUNIT_H

#include "Common.h"
#include "ComponentUnit.h"
#include "Address.h"

class HostUnit {

    const char *rootPath{};

    long id{};

    COMPONENT type{COMP_MAX};

    Address address[COMP_MAX]{};

public:

    HostUnit(COMPONENT, long);
    explicit HostUnit(COMPONENT, const char* = nullptr);
    HostUnit(COMPONENT, long, const char*);
    HostUnit(const HostUnit&);

    virtual ~HostUnit();

    const char* getRootPath() const;
    void setRootPath(const char*);

    long getID() const;
    void setID(long);

    ComponentUnit getUnit(COMPONENT);

    COMPONENT getType() const;
    void setType(COMPONENT);

    Address& getAddress(COMPONENT);
    void setAddress(COMPONENT, Address);

    template <class T>
    void setUIAddress(COMPONENT _out, T _address) {

        assert(_out != type);
        address[_out].setUI(_address);
    }

    template <class T>
    void setAllUIAddress(T _address) {

        address[ComponentUnit::next(type)].setUI(_address);
        address[ComponentUnit::prev(type)].setUI(_address);
    }

    void set(COMPONENT, long, COMPONENT, Address);
};

#endif //DICON_HOSTUNIT_H
