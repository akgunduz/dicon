//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#ifndef DICON_HOSTUNIT_H
#define DICON_HOSTUNIT_H

#include "Common.h"
#include "ComponentUnit.h"
#include "Address.h"

class HostUnit {

    const std::filesystem::path basePath;
    std::filesystem::path rootPath;

    uint32_t id{};

    COMPONENT type{COMP_MAX};

    Address address[COMP_MAX]{};

public:

    HostUnit(COMPONENT, long = 0);
    HostUnit(const HostUnit&);

    virtual ~HostUnit();

    const std::filesystem::path& getRootPath() const;
    void updateRootPath(long id);

    long getID() const;
    void setID(long);

    ARCH getArch() const;

    CommUnit getUnit(COMPONENT);

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

typedef std::shared_ptr<HostUnit> TypeHostUnit;

#endif //DICON_HOSTUNIT_H
