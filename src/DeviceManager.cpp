//
// Created by Haluk Akgunduz on 11/14/15.
//

#include "DeviceManager.h"
#include "NetAddress.h"
#include "Log.h"
#include "UnixSocketAddress.h"

DeviceManager* DeviceManager::instance;

DeviceManager::DeviceManager() {

    lastFreePort = DEFAULT_PORT;

    createDevices();
}

DeviceManager *DeviceManager::getInstance() {

    if (instance == nullptr) {
        instance = new DeviceManager();
    }

    return instance;
}

bool DeviceManager::createDevices() {

    struct ifaddrs* ifAddrStruct = nullptr;
    struct ifaddrs* loop = nullptr;

    getifaddrs(&ifAddrStruct);
    if (ifAddrStruct == nullptr) {
        return false;
    }

    for (loop = ifAddrStruct; loop != NULL; loop = loop->ifa_next) {

        if (loop->ifa_addr->sa_family == AF_INET) { // check it is IP4

            if (strncmp(loop->ifa_name, "et", 2) == 0 ||
                strncmp(loop->ifa_name, "en", 2) == 0 ||
                strncmp(loop->ifa_name, "br", 2) == 0 ||
                strncmp(loop->ifa_name, "lo", 2) == 0 ||
                strncmp(loop->ifa_name, "wlan", 2) == 0) {

                deviceList.push_back(Device(loop->ifa_name,
                                            ntohl(((struct sockaddr_in *) loop->ifa_addr)->sin_addr.s_addr),
                                            NetAddress::address2prefix(ntohl(((struct sockaddr_in *) loop->ifa_netmask)->sin_addr.s_addr))));

            }
        }
    };

    freeifaddrs(ifAddrStruct);

    deviceList.push_back(Device("us", INTERFACE_UNIXSOCKET));

    return true;
}

Device* DeviceManager::getFreeDevice(CONNECTTYPE connectType) {

    switch(connectType) {

        case CONNECT_TCP:
            return getFreeDeviceTCP();

        case CONNECT_LOOP:
            return getFreeDeviceLoopback();

        case CONNECT_UNIXSOCKET:
            return getFreeDeviceUnixSocket();

    }

    return nullptr;
}

Device *DeviceManager::getFreeDeviceTCP() {

    int netSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (netSocket < 0) {
        LOG_E("Socket open with err : %d!!!", errno);
        return nullptr;
    }

    int on = 1;
    if (setsockopt(netSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(int)) < 0) {
        LOG_E("Socket option with err : %d!!!", errno);
        close(netSocket);
        return nullptr;
    }

    for (int i = 0; i < getInstance()->deviceList.size(); i++) {

        Device *device = &getInstance()->deviceList[i];
        if (strncmp(device->name, "lo", 2) == 0 ||
            strncmp(device->name, "us", 2) == 0) {
            continue;
        }

        long address = NetAddress::parseAddress(device->getAddress(),
                                                DEFAULT_PORT, (int) device->getHelper());

        struct sockaddr_in serverAddress = NetAddress::getInetAddress(address);

        if (bind(netSocket, (struct sockaddr *)&serverAddress, sizeof(sockaddr_in)) < 0) {
            continue;
        }

        close(netSocket);

        device->setPort(DEFAULT_PORT);

        return device;
    }

    close(netSocket);

    return nullptr;
}

Device *DeviceManager::getFreeDeviceLoopback() {

    int netSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (netSocket < 0) {
        LOG_E("Socket open with err : %d!!!", errno);
        return nullptr;
    }

    int on = 1;
    if (setsockopt(netSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(int)) < 0) {
        LOG_E("Socket option with err : %d!!!", errno);
        close(netSocket);
        return nullptr;
    }

    for (int i = 0; i < getInstance()->deviceList.size(); i++) {

        Device *device = &getInstance()->deviceList[i];
        if (strncmp(device->name, "lo", 2) != 0) {
            continue;
        }

        int tryCount = 10;

        for (int j = tryCount; j > 0; j--) {

            long address = NetAddress::parseAddress(device->getAddress(),
                                                    getInstance()->lastFreePort, (int) device->getHelper());

            struct sockaddr_in serverAddress = NetAddress::getInetAddress(address);

            if (bind(netSocket, (struct sockaddr *) &serverAddress, sizeof(sockaddr_in)) < 0) {

                getInstance()->lastFreePort++;
                continue;
            }

            close(netSocket);

            device->setPort(getInstance()->lastFreePort);

            return device;
        }

        close(netSocket);

        return nullptr;

    }

    close(netSocket);

    return nullptr;
}


Device *DeviceManager::getFreeDeviceUnixSocket() {

    int unixSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (unixSocket < 0) {
        LOG_E("Socket receiver open with err : %d!!!", errno);
        return nullptr;
    }

    int on = 1;
    if (setsockopt(unixSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(int)) < 0) {
        LOG_E("Socket option with err : %d!!!", errno);
        close(unixSocket);
        return nullptr;
    }

    for (int i = 0; i < getInstance()->deviceList.size(); i++) {

        Device *device = &getInstance()->deviceList[i];
        if (strncmp(device->name, "us", 2) != 0) {
            continue;
        }

        int tryCount = 10;

        for (int j = tryCount; j > 0; j--) {

            long address = (((unsigned)getpid() << 10) & 0xFFFFFF) |  getInstance()->lastFreePort;

            sockaddr_un serverAddress = UnixSocketAddress::getUnixAddress(address);

            socklen_t len = offsetof(struct sockaddr_un, sun_path) + (uint32_t)strlen(serverAddress.sun_path);

            if (bind(unixSocket, (struct sockaddr *) &serverAddress, len) < 0) {

                getInstance()->lastFreePort++;
                continue;
            }

            close(unixSocket);

            device->setPort(getInstance()->lastFreePort);

            return device;
        }

        close(unixSocket);

        return nullptr;

    }

    close(unixSocket);

    return nullptr;
}

std::vector<Device> *DeviceManager::getDevices() {
    return &getInstance()->deviceList;
}
