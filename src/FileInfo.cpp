//
// Created by Haluk AKGUNDUZ on 23.04.2018.
//

#include "FileInfo.h"

FileItem *FileInfo::get() {

    return item;
}

void FileInfo::setState(long state) {

    this->state = state;
}

long FileInfo::getState() {

    return state;
}

FileInfo::FileInfo(FileItem *item, long state) {

    this->item = item;
    this->state = state;
}

void FileInfo::setFileListState(TypeFileInfoList *fileList, long state) {

    for (int i = 0; i < fileList->size(); i++) {

        fileList->at(i).setState(state);
    }

}

void FileInfo::setFileItemState(TypeFileInfoList *fileList, long id, long state) {

    for (int i = 0; i < fileList->size(); i++) {

        if (fileList->at(i).get()->getID() == id) {

            fileList->at(i).setState(state);
            break;
        }
    }

}

TypeFileInfoList FileInfo::getFileList(TypeFileInfoList *fileList, long state) {

    TypeFileInfoList list;

    for (int i = 0; i < fileList->size(); i++) {
        if (fileList->at(i).getState() & state) {
            list.push_back(fileList->at(i));
        }
    }

    return list;
}
