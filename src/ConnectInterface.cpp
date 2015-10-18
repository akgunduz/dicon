//
// Created by Haluk Akgunduz on 10/12/15.
//

#include "ConnectInterface.h"
#include "NetAddress.h"
#include "Net.h"
#include "UnixSocket.h"
#include "Pipe.h"

std::vector<ConnectInterface> ConnectInterface::interfaceList;

std::vector<ConnectInterface> ConnectInterface::getInterfaces() {

    if (interfaceList.size() > 0) {
        return interfaceList;
    }

    std::vector<ConnectInterface> netInterfaces = Net::getInterfaces();
    interfaceList.insert(interfaceList.end(), netInterfaces.begin(), netInterfaces.end());

    std::vector<ConnectInterface> unixSocketInterfaces = UnixSocket::getInterfaces();
    interfaceList.insert(interfaceList.end(), unixSocketInterfaces.begin(), unixSocketInterfaces.end());

    std::vector<ConnectInterface> pipeInterfaces = Pipe::getInterfaces();
    interfaceList.insert(interfaceList.end(), pipeInterfaces.begin(), pipeInterfaces.end());

    return interfaceList;
}


int ConnectInterface::getCount() {

    return (int)interfaceList.size();

}

ConnectInterface* ConnectInterface::getInterface(int index) {

    return &interfaceList[index >= interfaceList.size() ?
                          interfaceList.size() - 1 : index];

}

std::string ConnectInterface::getName(int index) {

    ConnectInterface *ci = &interfaceList[index >= interfaceList.size() ?
                                       interfaceList.size() - 1 : index];

    return ci->name;
}

INTERFACES ConnectInterface::getType(int index) {

    ConnectInterface *ci = &interfaceList[index >= interfaceList.size() ? interfaceList.size() - 1 : index];

    return ci->type;

}

long ConnectInterface::getAddress(int index) {

    ConnectInterface *ci = &interfaceList[index >= interfaceList.size() ?
                                       interfaceList.size() - 1 : index];

    return ci->address;
}

long ConnectInterface::getHelper(int index) {

    ConnectInterface *ci = &interfaceList[index >= interfaceList.size() ?
                                          interfaceList.size() - 1 : index];

    return ci->helper;
}