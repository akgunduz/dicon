//
// Created by akgunduz on 03.11.2015.
//

#include "FileList.h"

bool FileList::set(FileItem *item) {

    list[item->getMD5()->data] = item;
    return false;
}

bool FileList::check(FileItem *item) {
    return false;
}

bool FileList::check(Md5 *md5) {

    TypeFileList::iterator it = list.find(md5->data);
    return it != list.end();
}

FileItem* FileList::get(Md5 *md5) {
    return list[md5->data];
}

void FileList::setFlags(std::vector<Md5> *md5List, bool flag) {

    for (int i = 0; i < md5List->size(); i++) {
        FileItem *item = list[md5List->at(i).data];
        if (item != nullptr) {
            item->setFlaggedToSent(flag);

        } else {
            LOG_E("Serious problem, fix it!!!");
        }
    }
}
