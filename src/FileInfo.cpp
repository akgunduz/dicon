//
// Created by Haluk AKGUNDUZ on 23.04.2018.
//

#include "FileInfo.h"

FileItem *FileInfo::get() {

    return item;
}

void FileInfo::setState(uint8_t state) {

    this->state = state;
}

uint8_t FileInfo::getState() {

    return state;
}

FileInfo::FileInfo(FileItem *item, uint8_t state) {

    this->item = item;
    this->state = state;
}

TypeFileInfoList FileInfo::getFileList(TypeFileInfoList *fileList, uint8_t state) {

    TypeFileInfoList list;

    for (int i = 0; i < fileList->size(); i++) {
        if (fileList->at(i).getState() & state) {
            list.push_back(fileList->at(i));
        }
    }

    return list;
}
