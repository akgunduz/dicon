//
// Created by Haluk AKGUNDUZ on 25.09.2015.
// Copyright (c) 2015 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_ADDRESS_H
#define DICON_ADDRESS_H

#include <cstdint>
#include <memory>
#include <vector>
#include "CommInterfaceType.h"

#define INTERFACE_POS 0
#define INTERFACE_MASK 0x00FF
#define MULTICAST_POS 8
#define MULTICAST_MASK 0x0100
#define LOCALHOST_ADDRESS 0x7F000001

#define GET(a, b, c) (((a) & (c)) >> (b))
#define SET(a, b, c, d) (((a) & ~(d)) | ((((uint64_t)(b)) << (c)) & (d)))
#define MAP(a, b, c) ((c) - 1 - (((b) + (c) - (a)) % (c)))

struct BaseAddress {

    uint16_t port{};
    uint32_t base{};

    BaseAddress() = default;
    BaseAddress(uint32_t _base, uint16_t _port)
        : base(_base), port(_port) {}
};

class Address {

protected:

    uint16_t flag{};
    BaseAddress ui{};
    BaseAddress self{};

    friend class MessageHeader;
    friend struct std::less<Address>;

public:

    explicit Address(COMM_INTERFACE = COMMINTERFACE_TCPIP);
    Address(COMM_INTERFACE, uint32_t, uint16_t, bool = false);
    explicit Address(BaseAddress);
    Address(BaseAddress, BaseAddress);

    BaseAddress& get();
    BaseAddress& getUI();
    uint16_t& getFlag();

    void set(COMM_INTERFACE, uint32_t, uint16_t, bool = false);
    void setPort(uint16_t);
    void setUI(uint32_t, uint16_t);
    void setUI(uint16_t);
    void setUI();

    void set(BaseAddress&);
    void setUI(BaseAddress&);

    bool isMulticast();
    void setMulticast(bool);

    COMM_INTERFACE getInterface() const;
    void setInterface(COMM_INTERFACE);

    bool operator==(const Address& ref) const
    {
        return self.base == ref.self.base && self.port == ref.self.port;
    }
    bool operator!=(const Address& ref) const
    {
        return !(*this == ref);
    }

    bool operator<(const Address& ref) const
    {
        return ((uint64_t)self.port << 32UL | self.base) < ((uint64_t)ref.self.port << 32UL | ref.self.base);
    }

};

template<> struct std::less<Address>
{
    bool operator() (const Address& lhs, const Address& rhs) const
    {
        return ((uint64_t)lhs.self.port << 32UL | lhs.self.base) < ((uint64_t)rhs.self.port << 32UL | rhs.self.base);
    }
};

typedef std::shared_ptr<Address> TypeAddress;
typedef std::vector<TypeAddress> TypeAddressList;

#endif //DICON_ADDRESS_H
