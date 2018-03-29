//
// Created by akgunduz on 23.03.2018.
//

#include "Block.h"

Block::Block() {

    type = 0;
    count = 0;
    sizes = NULL;
}

Block::Block(int count, int type) {

    this->count = count;
    this->type = type;
    if (count > 0) {
        sizes = new int[count];
    }
}

Block::~Block() {

    if (count > 0) {
        delete[] sizes;
    }
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

    if (count > 0) {
        delete[] sizes;
    }

    this->count = count;
    sizes = new int[count];
}

int Block::getSize(int index) {

    return sizes[index];
}

void Block::setSize(int index, int size) {

    sizes[index] = size;
}

bool Block::isEnd() {

    return type == BLOCK_END_STREAM;
}
