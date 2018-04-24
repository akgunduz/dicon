//
// Created by akgunduz on 23.03.2018.
//

#include "Block.h"

Block::Block() {

    set(0, 0);
}

Block::Block(int count, int type) {

    set(std::min(count, BLOCK_EXT_SIZE_MAX), type);
}

Block::Block(Block& copy) {

    type = copy.type;
    count = copy.count;
    memcpy(sizes, copy.sizes, sizeof(int) * BLOCK_EXT_SIZE_MAX);
}

Block::~Block() {

}

int Block::getType() {

    return type;
}

void Block::setType(int type) {

    this->type = type;
}

int Block::getCount() {

    return count;
}

void Block::setCount(int count) {

    this->count = std::min(count, BLOCK_EXT_SIZE_MAX);
}

int Block::getSize(int index) {

    return sizes[std::min(index, BLOCK_EXT_SIZE_MAX)];
}

void Block::setSize(int index, int size) {

    sizes[std::min(index, BLOCK_EXT_SIZE_MAX)] = size;
}

bool Block::isEnd() {

    return type == BLOCK_END_STREAM;
}

void Block::set(int count, int type) {

    this->count = count;
    this->type = type;
    memset(this->sizes, 0, sizeof(int) * BLOCK_EXT_SIZE_MAX);
}
