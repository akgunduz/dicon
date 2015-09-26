//
// Created by Haluk AKGUNDUZ on 25/09/15.
//

#ifndef BANKOR_NETADDRESS_H
#define BANKOR_NETADDRESS_H

#include "Common.h"
#include "Address.h"

#define LOOPBACK_ADDRESS 0x7F000001
#define IPADDRESS_MASK 0xFFFFFFFF

class NetAddress : public Address {

    long debug_ip;
    int debug_port;

    sockaddr_in inet_addr;

public:

    NetAddress(long address);
    NetAddress(long ip, int port);
    NetAddress(const std::string &ip, int port);

    ~NetAddress(){}
    void init();

    virtual void set(long address);
    void set(long ip, int port);
    void set(const std::string &ip, int port);

    std::string getString();
    long parseIP(const std::string &ip);
    long getIP();
    void setIP(const std::string &ip);
    std::string getIPstr();
    int getPort();
    void setPort(int port);
    INTERFACES getInterface();
    sockaddr_in getInetAddress();
    bool isLoopback();
};


#endif //BANKOR_NETADDRESS_H
