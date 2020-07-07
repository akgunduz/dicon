//
// Created by Haluk AKGUNDUZ on 23.04.2018.
//

#include "FileInfo.h"

FileItem* FileInfo::get() const {

    return file;
}

void FileInfo::setOutput(bool _is_output) {

    this->is_output = _is_output;
}

bool FileInfo::isOutput() const {

    return is_output;
}

FileInfo::FileInfo(FileItem* file, bool is_output) {

    this->file = file;
    this->is_output = is_output;
}

TypeFileInfoList FileInfo::getFileList(const TypeFileInfoList &fileList, bool is_output) {

    TypeFileInfoList list;

    for (auto & file : fileList) {

        if (file.isOutput() & is_output) {

            list.push_back(file);
        }
    }

    return list;
}


void FileInfo::setFileListState(TypeFileInfoList &fileList, bool is_output) {

    for (auto & file : fileList) {

        file.setOutput(is_output);
    }
}

TypeFileInfoList FileInfo::getReqFileList(const ComponentObject& component, const TypeFileInfoList& list) {

    TypeFileInfoList reqList;

    for (auto & file : list) {

        if (!Util::checkPath(component.getRootPath(),
                             file.get()->getJobDir(), file.get()->getFileName(), false)) {

            reqList.push_back(file);

        } else {

            Md5 md5(Util::getAbsMD5Path(component.getRootPath(),
                                        file.get()->getJobDir(), file.get()->getFileName()).c_str());
            if (!md5.equal(file.get()->getMD5())) {

                reqList.push_back(file);
            }
        }
    }

    return reqList;
}
