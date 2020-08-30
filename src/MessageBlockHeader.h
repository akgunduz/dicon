//
// Created by akgunduz on 23.03.2018.
//

#ifndef DICON_MESSAGEBLOCKHEADER_H
#define DICON_MESSAGEBLOCKHEADER_H

#include "Common.h"
#include "MessageType.h"

class MessageBlockHeader {

    uint16_t type{};
    uint16_t count{};
    uint32_t data[3]{};

public:
    MessageBlockHeader(const MessageBlockHeader&) = default;
    explicit MessageBlockHeader(uint16_t = BLOCK_END_STREAM);
    ~MessageBlockHeader() = default;

    uint16_t getType();
    void setType(uint16_t);

    bool isEnd();

    uint16_t getCount();
    void setCount(uint16_t);

    uint32_t get(int index);
    uint16_t add(uint32_t);
    void set(int, uint32_t);
};

#endif //DICON_MESSAGEBLOCKHEADER_H
