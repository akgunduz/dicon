//
// Created by akgunduz on 23.03.2018.
//

#ifndef BANKOR_BLOCK_H
#define BANKOR_BLOCK_H

#include "Common.h"

#define BLOCK_END_STREAM 0xFFFF

#define BLOCK_HEADER_SIZE 8

#define BLOCK_EXT_SIZE_MAX 3

class Block {

    int type;
    int count;
    int sizes[BLOCK_EXT_SIZE_MAX];

public:
    Block();
    Block(Block&);
    Block(int, int = BLOCK_END_STREAM);
    ~Block();

    void set(int, int);

    int getType();
    void setType(int);

    int getCount();
    void setCount(int);

    int getSize(int);
    void setSize(int, int);

    bool isEnd();
};


#endif //BANKOR_BLOCK_H
