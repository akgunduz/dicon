//
// Created by akgunduz on 23.03.2018.
//

#include "MessageBlockHeader.h"

MessageBlockHeader::MessageBlockHeader(uint16_t _type) :
    type(_type) {
}

uint16_t MessageBlockHeader::getType() {

    return type;
}

void MessageBlockHeader::setType(uint16_t _type) {

    type = _type;
}

bool MessageBlockHeader::isEnd() {

    return type == BLOCK_END_STREAM;
}

uint16_t MessageBlockHeader::getCount() {

    return count;
}

uint64_t MessageBlockHeader::get(int index) {

    return data[index];
}

uint16_t MessageBlockHeader::add(uint64_t _data) {

    data.emplace_back(_data);
    count++;
    return count;
}
