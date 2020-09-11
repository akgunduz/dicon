//
// Created by z003sc8r on 9/11/2020.
//

#ifndef DICON_HOST_H
#define DICON_HOST_H

#include "HostUnit.h"

class Host {

    TypeHostUnit self;

    Address address[COMP_MAX]{};

public:

    Host(COMPONENT _type, TypeID _id);

    virtual ~Host() = 0;

    Address& getAddress(COMPONENT);
    void setAddress(COMPONENT, Address);

    TypeHostUnit& get();
    TypeHostUnit get(COMPONENT);

    const std::filesystem::path& getRootPath();

    void setID(TypeID);

    template <class T>
    void setUIAddress(COMPONENT _out, T _address) {

        assert(_out != self->getType());
        address[_out].setUI(_address);
    }

    template <class T>
    void setAllUIAddress(T _address) {

        address[COMP_DISTRIBUTOR].setUI(_address);
        address[COMP_COLLECTOR].setUI(_address);
        address[COMP_NODE].setUI(_address);
    }
};

typedef std::shared_ptr<Host> TypeHost;

#endif //DICON_HOST_H
