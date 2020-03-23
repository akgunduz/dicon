//
// Created by Haluk AKGUNDUZ on 23.04.2018.
//

#ifndef DICON_FILEINFO_H
#define DICON_FILEINFO_H

#include "FileItem.h"

//#define FILEINFO_EXIST      0x01
//#define FILEINFO_NONEXIST   0x02
//#define FILEINFO_NONOUTPUT  (FILEINFO_EXIST | FILEINFO_NONEXIST)
//#define FILEINFO_OUTPUT     0x04
//#define FILEINFO_ALL        (FILEINFO_NONOUTPUT | FILEINFO_OUTPUT)

#define FILEINFO_OUTPUT 1
#define FILEINFO_NONOUTPUT ~FILEINFO_OUTPUT

class FileInfo;

typedef std::vector<FileInfo> TypeFileInfoList;

class FileInfo {

    FileItem *file;
    bool is_output;

public:

    FileInfo(FileItem *, bool);
    FileItem* get();

    bool isOutput();
    void setOutput(bool);

    static TypeFileInfoList getFileList(TypeFileInfoList*, bool);
    static void setFileListState(TypeFileInfoList*, bool);
//    static void setFileItemState(TypeFileInfoList*, long, bool);
//    static bool isInclude(TypeFileInfoList*, FileItem*);
    static TypeFileInfoList checkFileExistence(ComponentObject, TypeFileInfoList*);
};


#endif //DICON_FILEINFO_H
