//
// Created by akgunduz on 03.11.2015.
//

#ifndef BANKOR_FILELIST_H
#define BANKOR_FILELIST_H


#include "FileItem.h"

struct cmp_str {
    bool operator()(const uint8_t *a, const uint8_t *b) const {
        return memcmp(a, b, MD5_DIGEST_LENGTH) < 0;
    }
};

typedef std::map<const uint8_t*, FileItem*, cmp_str> TypeFileList;

enum FILELIST_FLAG {
    FILE_LIST_ALL,
    FILE_LIST_TRUE,
    FILE_LIST_FALSE,
};

class Message;

typedef bool (Message::*fWriteProcess)(int desc, FileItem *content);

class FileList {

    char parentDir[50];
    TypeFileList list;

public:

    FileList(const char* parentDir);
    bool set(FileItem*);
    bool check(FileItem*);
    bool check(Md5*);
    int getCount();
    const char* getDir();
    FileItem* get(Md5*);
    void setFlags(std::vector<Md5> *, bool);
    void remove(std::vector<Md5> *);
    int process(Message *msg, fWriteProcess process, FILELIST_FLAG flag);
};


#endif //BANKOR_FILELIST_H
