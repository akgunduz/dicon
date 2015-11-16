//
// Created by akgunduz on 03.11.2015.
//

#include "FileList.h"
#include "Message.h"

FileList::FileList(long id, const char *parentDir) {

    this->id = id;
    strcpy(this->parentDir, parentDir);
}

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

    for (unsigned int i = 0; i < md5List->size(); i++) {
        TypeFileList::iterator itr = list.find(md5List->at(i).data);

        if (itr != list.end()) {
            itr->second->setFlaggedToSent(flag);

        } else {
            LOG_E("Serious problem, fix it!!!");
        }
    }
}

void FileList::remove(std::vector<Md5> *md5List) {

    for (unsigned int i = 0; i < md5List->size(); i++) {
        TypeFileList::iterator itr = list.find(md5List->at(i).data);

        if (itr != list.end()) {
            list.erase(itr);

        } else {
            LOG_E("Serious problem, fix it!!!");
        }
    }
}

int FileList::getCount() {
    return (int) list.size();
}

int FileList::process(Message *msg, fWriteProcess function, FILELIST_FLAG flag) {

    int contentCount = 0;

    for (TypeFileList::iterator it = list.begin(); it != list.end(); it++) {

        FileItem *item = it->second;

        if (flag == FILE_LIST_ALL || (flag == FILE_LIST_TRUE && item->isFlaggedToSent())
                                      || (flag == FILE_LIST_FALSE && !item->isFlaggedToSent())) {

            (msg->*function)(msg->getDescriptor(), item);
            contentCount++;
        }
    }
    return contentCount;
}


const char *FileList::getDir() {
    return parentDir;
}

long FileList::getID() {
    return id;
}
