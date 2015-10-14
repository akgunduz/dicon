//
// Created by Haluk Akgunduz on 10/14/15.
//

#include "Component.h"

Component::Component(int interfaceIndexFirst, int interfaceIndexSecond, const char* rootPath) {

    callback = new InterfaceCallback(receiveCB, this);

    try {

        connectors[0] = new Connector(interfaceIndexFirst, callback, rootPath);

    } catch (const std::runtime_error e) {

        LOG_E("Connector Init failed!!!");

        delete callback;

        throw std::runtime_error("Distributor : Connector Init failed!!!");
    }

    if (interfaceIndexFirst != interfaceIndexSecond) {

        try {

            connectors[1] = new Connector(interfaceIndexSecond, callback, rootPath);

        } catch (const std::runtime_error e) {

            LOG_E("Connector Init failed!!!");

            delete connectors[0];
            delete callback;

            throw std::runtime_error("Distributor : Connector Init failed!!!");
        }

    } else {

        connectors[1] = connectors[0];

    }
}

Component::~Component() {

    if (connectors[1] != connectors[0]) {
        delete connectors[1];
    }

    delete connectors[0];

    delete callback;
}

const char* Component::getRootPath() {

    return connectors[0]->getRootPath();

}

bool Component::receiveCB(void *arg, long address, Message *msg) {

    Component *component = (Component *) arg;

    return component->onReceive(address, msg);
}