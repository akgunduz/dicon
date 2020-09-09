//
// Created by z003sc8r on 9/9/2020.
//

#ifndef DICON_COMMDATA_H
#define DICON_COMMDATA_H

#include "Message.h"
#include "CommInterface.h"

class CommData {

    TypeMessage msg;
    TypeComponentUnit component;
    TypeInterface commInterface;

public:

    CommData(const TypeInterface& _interface) {

        commInterface = _interface;
        component = std::make_shared<ComponentUnit>();
        msg = std::make_unique<Message>(_interface->getHost());
    }

    CommData(const TypeInterface& _interface, const TypeComponentUnit& _component, TypeMessage& _msg) {

        commInterface = _interface;
        component = _component;
        msg = std::move(_msg);
    }

    void reInitialize() {

        component = std::make_shared<ComponentUnit>();
        msg = std::make_unique<Message>(commInterface->getHost());
    }

    TypeInterface& getInterface() {

        return commInterface;
    }

    TypeComponentUnit getComponent() {

        return component;
    }

    TypeMessage& getMsg() {

        return msg;
    }
};


#endif //DICON_COMMDATA_H
