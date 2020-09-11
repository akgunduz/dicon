//
// Created by z003sc8r on 9/11/2020.
//

#include "Host.h"

Host::Host(COMPONENT _type, TypeID _id) {

    self = std::make_unique<HostUnit>(_type, _id);
}

Host::~Host()
{
}

Address &Host::getAddress(COMPONENT _out) {

//    assert(_out != self->getType());

    return commInterface[_out]->getAddress();
}

//void Host::setAddress(COMPONENT _out, Address _address) {
//
//    assert(_out != self->getType());
//
//    commInterface[_out]->setAddress(_address);
//}

TypeHostUnit& Host::get() {

    return self;
}

TypeHostUnit Host::get(COMPONENT _out) {

    TypeHostUnit unit = std::make_unique<HostUnit>(*self);

    unit->setAddress(getAddress(_out));

    return std::move(unit);
}

const std::filesystem::path& Host::getRootPath() {

    return self->getRootPath();
}

void Host::setID(TypeID _id) {

    self->setID(_id);
}

void Host::setInterface(COMPONENT _type, const TypeInterface& _interface) {

    commInterface[_type] = _interface;

}
