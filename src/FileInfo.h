//
// Created by Haluk AKGUNDUZ on 23.04.2018.
//

#ifndef BANKOR_FILEINFO_H
#define BANKOR_FILEINFO_H

#include "FileItem.h"

#define FILEINFO_EXIST      0x01
#define FILEINFO_NONEXIST   0x02
#define FILEINFO_NONOUTPUT  (FILEINFO_EXIST | FILEINFO_NONEXIST)
#define FILEINFO_OUTPUT     0x04
#define FILEINFO_ALL        (FILEINFO_NONOUTPUT | FILEINFO_OUTPUT)

class FileInfo;

typedef std::vector<FileInfo> TypeFileInfoList;

class FileInfo {

    FileItem *item;
    uint8_t state;

public:

    FileInfo(FileItem *, uint8_t);
    FileItem* get();

    uint8_t getState();
    void setState(uint8_t);

    static TypeFileInfoList getFileList(TypeFileInfoList*, uint8_t);
};



class FileInfoList {

    TypeFileInfoList depFileList;
    TypeFileInfoList outFileList;

public:

    bool add(FileInfo);
    bool switchType(FileInfo);
};




#endif //BANKOR_FILEINFO_H
