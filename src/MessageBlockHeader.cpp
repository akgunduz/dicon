//
// Created by akgunduz on 23.03.2018.
//

#include "MessageBlockHeader.h"

MessageBlockHeader::MessageBlockHeader(int _type) :
    type(_type) {
}

MessageBlockHeader::MessageBlockHeader(const MessageBlockHeader& copy) {

    type = copy.type;
    count = copy.count;
    data = copy.data;
}

MessageBlockHeader::~MessageBlockHeader() = default;

int MessageBlockHeader::getType() const {

    return type;
}

void MessageBlockHeader::setType(int _type) {

    type = _type;
}

bool MessageBlockHeader::isEnd() const {

    return type == BLOCK_END_STREAM;
}

int MessageBlockHeader::getCount() const {

    return count;
}

long MessageBlockHeader::get(int index) {

    return data[index];
}

int MessageBlockHeader::add(long _data) {

    data.emplace_back(_data);
    count++;
    return count;
}
