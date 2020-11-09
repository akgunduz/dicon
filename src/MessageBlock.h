//
// Created by Haluk AKGUNDUZ on 23.03.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_MESSAGEBLOCK_H
#define DICON_MESSAGEBLOCK_H

#include <cstdint>
#include <cstddef>

#define SIGNATURE 0x55AA

enum MSG_HEADER {

    MSGHEADER_HEADER = 0xA1,
    MSGHEADER_NUMBER,
    MSGHEADER_STRING,
    MSGHEADER_PATH,
    MSGHEADER_BINARY,
    MSGHEADER_END,
    MSGHEADER_MAX = 0xFF
};

class MessageBlock {

    uint16_t sign {SIGNATURE};
    uint16_t type {MSGHEADER_MAX};
    uint32_t size {};

public:
    MessageBlock(uint16_t, uint32_t);
    ~MessageBlock() = default;

    void set(uint16_t, uint32_t);

    MSG_HEADER getType();
    size_t getSize();
    uint16_t getSign();
    const char* getName();

    bool fillBuffer(uint8_t*) const;
    bool parseBuffer(const uint8_t*);
};

#endif //DICON_MESSAGEBLOCK_H
