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
    std::vector<uint64_t> data;

public:
    MessageBlockHeader(const MessageBlockHeader&) = default;
    explicit MessageBlockHeader(uint16_t = BLOCK_END_STREAM);
    ~MessageBlockHeader() = default;

    uint16_t getType();
    void setType(uint16_t);

    bool isEnd();

    uint16_t getCount();

    uint64_t get(int index);
    uint16_t add(uint64_t _data);
};

#endif //DICON_MESSAGEBLOCKHEADER_H
