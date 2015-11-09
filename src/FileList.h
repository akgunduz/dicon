//
// Created by akgunduz on 03.11.2015.
//

#ifndef BANKOR_FILELIST_H
#define BANKOR_FILELIST_H


#include "FileItem.h"

struct cmp_str {
    bool operator()(const uint8_t *a, const uint8_t *b) {
        return memcmp(a, b, MD5_DIGEST_LENGTH) < 0;
    }
};

typedef std::map<const uint8_t*, FileItem*, cmp_str> TypeFileList;

class FileList {

    TypeFileList list;

public:
    bool set(FileItem*);
    bool check(FileItem*);
    bool check(Md5*);
    FileItem* get(Md5*);
    void setFlags(std::vector<Md5> *, bool);
};


#endif //BANKOR_FILELIST_H
