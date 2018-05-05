//
// Created by Haluk AKGUNDUZ on 23.04.2018.
//

#include "FileInfo.h"

FileItem* FileInfo::get() {

    return file;
}

void FileInfo::setOutput(bool is_output) {

    this->is_output = is_output;
}

bool FileInfo::isOutput() {

    return is_output;
}

FileInfo::FileInfo(FileItem* file, bool is_output) {

    this->file = file;
    this->is_output = is_output;
}

TypeFileInfoList FileInfo::getFileList(TypeFileInfoList *fileList, bool is_output) {

    TypeFileInfoList list;

    for (int i = 0; i < fileList->size(); i++) {

        if (fileList->at(i).isOutput() & is_output) {

            list.push_back(fileList->at(i));
        }
    }

    return list;
}


void FileInfo::setFileListState(TypeFileInfoList *fileList, bool is_output) {

    for (int i = 0; i < fileList->size(); i++) {

        fileList->at(i).setOutput(is_output);
    }

}

TypeFileInfoList FileInfo::checkFileExistence(ComponentObject component, TypeFileInfoList *list) {

    TypeFileInfoList reqList;

    for (int i = 0; i < list->size(); i++) {

        if (!Util::checkPath(component.getRootPath(),
                             list->at(i).get()->getJobDir(), list->at(i).get()->getFileName(), false)) {

            reqList.push_back(list->at(i));

        } else {

            Md5 md5(Util::getAbsMD5Path(component.getRootPath(),
                                        list->at(i).get()->getJobDir(), list->at(i).get()->getFileName()).c_str());
            if (!md5.equal(list->at(i).get()->getMD5())) {

                reqList.push_back(list->at(i));
            }
        }
    }

    return reqList;
}
