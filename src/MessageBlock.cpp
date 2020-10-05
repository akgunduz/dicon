//
// Created by akgunduz on 23.03.2018.
//

#include "MessageBlock.h"
#include "Platform.h"

MessageBlock::MessageBlock(uint16_t _type, uint32_t _size) :
    type(_type), size(_size) {
}

void MessageBlock::set(uint16_t _type, uint32_t _size) {

    type = _type;
    size = _size;
}

uint16_t MessageBlock::getSign() {

    return sign;
}

MSG_HEADER MessageBlock::getType() {

    return (MSG_HEADER)type;
}

size_t MessageBlock::getSize() {

    return size;
}

bool MessageBlock::fillBuffer(uint8_t* buffer) const {

    if (!buffer) {

        return false;
    }

    *((uint16_t *) buffer) = htons(sign); buffer += 2;
    *((uint16_t *) buffer) = htons(type); buffer += 2;
    *((uint32_t *) buffer) = htonl(size);

    return true;
}

bool MessageBlock::parseBuffer(const uint8_t *buffer) {

    if (!buffer) {

        return false;
    }

    sign = ntohs(*((uint16_t *) buffer)); buffer += 2;
    type = ntohs(*((uint16_t *) buffer)); buffer += 2;
    size = ntohl(*((uint32_t *) buffer));

    return true;
}
