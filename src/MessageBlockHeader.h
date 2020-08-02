//
// Created by akgunduz on 23.03.2018.
//

#ifndef DICON_MESSAGEBLOCKHEADER_H
#define DICON_MESSAGEBLOCKHEADER_H

#include "Common.h"
#include "MessageTypes.h"

#define BLOCK_HEADER_SIZE 8

class MessageBlockHeader {

    int type{};
    int count{};
    std::vector<long> data;

public:
    MessageBlockHeader(const MessageBlockHeader&);
    explicit MessageBlockHeader(int = BLOCK_END_STREAM);
    ~MessageBlockHeader();

    int getType() const;
    void setType(int);

    bool isEnd() const;

    int getCount() const;

    long get(int index);
    int add(long _data);
};

#endif //DICON_MESSAGEBLOCKHEADER_H
