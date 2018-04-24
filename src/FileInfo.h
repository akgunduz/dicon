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
    long state;

public:

    FileInfo(FileItem *, long);
    FileItem* get();

    long getState();
    void setState(long);

    static TypeFileInfoList getFileList(TypeFileInfoList*, long);
    static void setFileListState(TypeFileInfoList*, long);
    static void setFileItemState(TypeFileInfoList*, long, long);
};


#endif //BANKOR_FILEINFO_H
