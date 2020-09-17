//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#ifndef DICON_HOSTUNIT_H
#define DICON_HOSTUNIT_H

#include "Address.h"
#include "ComponentUnit.h"

class HostUnit : public BaseUnit {

    const std::filesystem::path basePath;
    std::filesystem::path rootPath;

    TypeAddress address[COMP_MAX]{};

public:

    explicit HostUnit(COMPONENT, TypeID = 0);
    HostUnit(const HostUnit&);
    explicit HostUnit(const HostUnit*);

    ~HostUnit() override;

    const std::filesystem::path& getRootPath();

    void setID(TypeID) final;

    TypeAddress& getAddress(COMPONENT);
    void setAddress(COMPONENT, TypeAddress, bool = false);

    TypeComponentUnit forkComponent(COMPONENT);
    TypeComponentUnit forkComponent();
    TypeCommUnit forkCommUnit(COMPONENT);
    TypeCommUnit forkCommUnit();
};

typedef std::shared_ptr<HostUnit> TypeHostUnit;

#endif //DICON_HOSTUNIT_H
