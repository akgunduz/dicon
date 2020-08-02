//
// Created by Haluk AKGUNDUZ on 25/09/15.
//

#ifndef DICON_ADDRESS_H
#define DICON_ADDRESS_H

#include "InterfaceTypes.h"

#define INTERFACE_POS 0
#define INTERFACE_MASK 0x00FF
#define MULTICAST_POS 8
#define MULTICAST_MASK 0x0100

#define GET(a, b, c) (((a) & (c)) >> (b))
#define SET(a, b, c, d) (((a) & ~(d)) | ((((uint64_t)(b)) << (c)) & (d)))

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
    uint16_t socket{};
    BaseAddress ui{};
    BaseAddress self{};

    friend class MessageHeader;
    friend struct std::less<Address>;

public:

    static Address invalid;

    Address(INTERFACE = INTERFACE_NET);
    Address(uint32_t, uint16_t, INTERFACE = INTERFACE_NET);
    Address(BaseAddress);
    Address(BaseAddress, BaseAddress);

    BaseAddress& get();
    BaseAddress& getUI();

    void set(uint32_t, uint16_t);
    void setUI(uint32_t, uint16_t);

    void set(BaseAddress&);
    void setUI(BaseAddress&);

    uint16_t getSocket();
    bool isMulticast();
    INTERFACE getInterface() const;

    void setSocket(uint16_t);
    void setMulticast(bool);
    void setInterface(INTERFACE);

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
        return ((uint64_t)self.port << 32 | self.base) < ((uint64_t)ref.self.port << 32 | ref.self.base);
    }

//    static INTERFACE getInterface(long&);
//    static long getBase(long&);
//    static int getPort(long&);
//    static int getSocket(long&);
//    static void setSocket(long&, int);
//    static bool isMulticast(long&);
//    static void setMulticast(long&, bool);
//
//    static long create(INTERFACE, long, int, int = 0, bool = 0);
};

template<> struct std::less<Address>
{
    bool operator() (const Address& lhs, const Address& rhs) const
    {
        return ((uint64_t)lhs.self.port << 32 | lhs.self.base) < ((uint64_t)rhs.self.port << 32 | rhs.self.base);
    }
};

typedef std::vector<Address> TypeAddressList;

#endif //DICON_ADDRESS_H
