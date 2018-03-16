//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include <openssl/md5.h>
#include "Message.h"

//Message::Message(Unit host, const char* rootPath)
//		: BaseMessage(host) {
//
//    setRootPath(rootPath);
//}
//
//Message::Message(Unit owner, int type, const char* rootPath)
//		: BaseMessage(owner, type) {
//
//    setRootPath(rootPath);
//}
Message::Message(Unit host)
		: BaseMessage(host) {

}

Message::Message(Unit owner, int type)
		: BaseMessage(owner, type) {

}

//const char *Message::getRootPath() {
//	return rootPath;
//}
//
//void Message::setRootPath(const char *rootPath) {
//    strcpy(this->rootPath, rootPath);
//}


void Message::setJob(int streamFlag, FileList *fileList) {

    setStreamFlag(streamFlag);

    this->fileList = fileList;
}

bool Message::readMD5(int desc, Md5* md5) {

	if (!readBlock(desc, md5->data, MD5_DIGEST_LENGTH)) {
		LOG_E("Can not read MD5 from stream");
		return false;
	}

	return true;
}

bool Message::readFileBinary(int desc, FileItem *content, BlockHeader *header) {

	if (header->blockType != BLOCK_FILE_BINARY) {
		LOG_E("readFileBinary can not read other blocks");
		return false;
	}

    char jobDir[MAX_JOB_DIR_LENGTH];
    if (!readString(desc, jobDir, header->sizes[0])) {
        LOG_E("readFileBinary can not read path data");
        return false;
    }

	char fileName[MAX_FILE_NAME_LENGTH];
	if (!readString(desc, fileName, header->sizes[1])) {
		LOG_E("readFileBinary can not read path data");
		return false;
	}

    long fileType;
    if (!readNumber(desc, &fileType)) {
        LOG_E("readFileBinary can not read filetype data");
        return false;
    }

    content->setFile(getOwner(), jobDir, fileName, (FILETYPE)fileType);

    Md5 calcMD5;
	if (!readBinary(desc, content->getRefPath(), &calcMD5,
                    content->getMD5Path(), header->sizes[2])) {
		LOG_E("readFileBinary can not read Binary data");
		return false;
	}

    Md5 md5;
    if (!readMD5(desc, &md5)) {
        LOG_E("readFileBinary can not read MD5 data");
        return false;
    }

	if (!md5.equal(&calcMD5)) {
		LOG_E("readFileBinary mismatch in md5 data");
		return false;
	}

    content->setMD5(&md5);

	return true;
}

bool Message::readFileMD5(int desc, Md5 *content, BlockHeader* header) {

	if (header->sizes[0] != MD5_DIGEST_LENGTH) {
		LOG_E("Md5 size must be %d long", MD5_DIGEST_LENGTH);
		return false;
	}

	if (!readMD5(desc, content)) {
		LOG_E("Can not read md5");
		return false;
	}

	return true;
}

bool Message::readJobInfo(int desc, char *jobDir, BlockHeader *header) {

    if (!readString(desc, jobDir, header->sizes[0])) {
        LOG_E("readFileBinary can not read path data");
        return false;
    }

    return true;
}


bool Message::readMessageBlock(int in, BlockHeader *blockHeader) {

    switch(blockHeader->blockType) {

        case BLOCK_FILE_BINARY: {

            FileItem *fileItem = new FileItem(getHost());

            if (!readFileBinary(in, fileItem, blockHeader)) {
                return false;
            }

            LOG_E("New binary with path %s/%s", fileItem->getJobDir(), fileItem->getFileName());

            delete fileItem;
        }
            break;

        case BLOCK_FILE_MD5: {

            Md5 md5;
            if (!readFileMD5(in, &md5, blockHeader)) {
                return false;
            }

            md5List.push_back(md5);
        }
            break;

        case BLOCK_JOB_INFO:

            if (!readJobInfo(in, jobDir, blockHeader)) {
                return false;
            }

            break;

        default:
            return false;
    }

    return true;
}

bool Message::readFinalize() {

   /* if (job != nullptr) {
        job->parse();
    }*/
    return true;
}

bool Message::writeMD5(int desc, Md5* md5) {

	if (!writeBlock(desc, md5->data, MD5_DIGEST_LENGTH)) {
		LOG_E("Can not write md5 to stream");
		return false;
	}

	return true;
}

bool Message::writeFileBinary(int desc, FileItem *content) {

	BlockHeader blockHeader = {3, BLOCK_FILE_BINARY};

    blockHeader.sizes[0] = (uint32_t)strlen(content->getJobDir());
    blockHeader.sizes[1] = (uint32_t)strlen(content->getFileName());
    blockHeader.sizes[2] = getBinarySize(Util::absPath(getHost(), content->getRefPath()).c_str());

	if (!writeBlockHeader(desc, &blockHeader)) {
		return false;
	}

    if (!writeString(desc, content->getJobDir())) {
        return false;
    }

	if (!writeString(desc, content->getFileName())) {
        return false;
    }

    if (!writeNumber(desc, content->getFileType())) {
        return false;
    }

	Md5 calcMD5;
	if (!writeBinary(desc, content->getRefPath(), &calcMD5, blockHeader.sizes[2])) {
		LOG_E("writeFileBinary can not write Binary data");
		return false;
	}

    content->setMD5(&calcMD5);

	if (!writeMD5(desc, &calcMD5)) {
        LOG_E("writeFileBinary can not write md5 data");
		return false;
	}

	return true;

}

bool Message::writeFileMD5(int desc, FileItem *content) {

    BlockHeader blockHeader = {1, BLOCK_FILE_MD5};

    blockHeader.sizes[0] = MD5_DIGEST_LENGTH;

    if (!writeBlockHeader(desc, &blockHeader)) {
        return false;
    }

    return writeMD5(desc, content->getMD5());
}

bool Message::writeJobInfo(int desc, const char *jobDir) {

    BlockHeader blockHeader = {1, BLOCK_JOB_INFO};

    blockHeader.sizes[0] = (int) strlen(jobDir);

    if (!writeBlockHeader(desc, &blockHeader)) {
        return false;
    }

    return writeString(desc, jobDir);
}

bool Message::writeMessageStream(int out, int streamFlag) {

    int contentCount;

    switch(streamFlag) {

        case STREAM_JOB:

            if (!writeJobInfo(out, fileList->getDir())) {
                return false;
            }

            contentCount = fileList->process(this, &Message::writeFileBinary, FILE_LIST_ALL);

            LOG_E("%d job content sent to network", contentCount);
            break;

        case STREAM_BINARY:
/*
            contentCount = 0;

            for (int j = 0; j < job->getContentCount(CONTENT_FILE); j++) {
                Rule *rule = (Rule *)job->getContent(CONTENT_FILE, j);
                for (int i = 0; i < rule->getContentCount(CONTENT_FILE); i++) {
                    FileItem *content = (FileItem *)rule->getContent(CONTENT_FILE, i);
                    if (content->isFlaggedToSent()) {
                        if (!writeFileBinary(out, content)) {
                            return false;
                        }
                        contentCount++;
                    }
                }
            }
            LOG_E("%d binary content sent to network", contentCount);*/

            contentCount = fileList->process(this, &Message::writeFileBinary, FILE_LIST_TRUE);

            LOG_E("%d binary content sent to network", contentCount);

            break;

        case STREAM_MD5ONLY:
/*
            contentCount = 0;

            for (int j = 0; j < job->getContentCount(CONTENT_FILE); j++) {
                Rule *rule = (Rule *)job->getContent(CONTENT_FILE, j);
                for (int i = 0; i < rule->getContentCount(CONTENT_FILE); i++) {
                    FileItem *content = (FileItem *)rule->getContent(CONTENT_FILE, i);
                    if (content->isFlaggedToSent()) {
                        if (!writeFileMD5(out, content)) {
                            return false;
                        }
                        contentCount++;
                    }
                }
            }
*/
            contentCount = fileList->process(this, &Message::writeFileMD5, FILE_LIST_TRUE);

            LOG_E("%d md5 content sent to network", contentCount);
            break;

        default :
            break;
    }

    return true;

}

bool Message::writeFinalize() {
    return true;
}

const char *Message::getJobDir() {
    return jobDir;
}
