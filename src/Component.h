//
// Created by Haluk Akgunduz on 10/14/15.
//

#ifndef BANKOR_COMPONENT_H
#define BANKOR_COMPONENT_H

#include "Connector.h"

class Component {

protected :

    InterfaceCallback *callback;
    Connector *connectors[2];

public:
    Component(int interfaceIndexFirst, int interfaceIndexSecond, const char* rootPath);
    ~Component();
    const char* getRootPath();
    virtual INTERFACES getInterfaceType(HOST host) = 0;
    static bool receiveCB(void *, long, Message *);
    virtual bool onReceive(long, Message *) = 0;

};


#endif //BANKOR_COMPONENT_H
