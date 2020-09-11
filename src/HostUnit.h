//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#ifndef DICON_HOSTUNIT_H
#define DICON_HOSTUNIT_H

#include "CommUnit.h"
#include "Address.h"

class HostUnit : public CommUnit {

    const std::filesystem::path basePath;
    std::filesystem::path rootPath;

public:

    explicit HostUnit(COMPONENT, TypeID = 0);
    HostUnit(const HostUnit&);

    virtual ~HostUnit();

    const std::filesystem::path& getRootPath();

    void setID(TypeID) final;
};

typedef std::unique_ptr<HostUnit> TypeHostUnit;

#endif //DICON_HOSTUNIT_H
