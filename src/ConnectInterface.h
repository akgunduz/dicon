//
// Created by Haluk Akgunduz on 10/12/15.
//

#ifndef BANKOR_CONNECTINTERFACE_H
#define BANKOR_CONNECTINTERFACE_H

#include "Common.h"

class ConnectInterface {
public:
    std::string name;
    INTERFACES type;

    long address;
    long helper;

    static std::vector<ConnectInterface>interfaceList;

    ConnectInterface(std::string _name, INTERFACES _type) :
    name(_name), type(_type), address(0), helper(0)  {};
    ConnectInterface(std::string _name, long _ipAddress, long _helper) :
    name(_name), type(INTERFACE_NET), address(_ipAddress), helper(_helper) {};

    static std::vector<ConnectInterface> getInterfaces();
    static int getCount();
    static ConnectInterface* getInterface(int);
    static std::string getName(int);
    static INTERFACES getType(int);
    static long getAddress(int);
    static long getHelper(int);
};


#endif //BANKOR_CONNECTINTERFACE_H
