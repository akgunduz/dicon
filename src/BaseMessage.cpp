//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "BaseMessage.h"
#include "Tools.h"

BaseMessage::BaseMessage() {

	setStreamFlag(STREAM_NONE);
	setPriority(DEFAULT_PRIORITY);
}

BaseMessage::BaseMessage(long time, long deviceID, int type) {

	header.time = time;
	header.deviceID = deviceID;
	header.messageID = 0;//new Random(time).nextLong();
	header.type = type;

	setStreamFlag(STREAM_NONE);
	setPriority(DEFAULT_PRIORITY);
}

void BaseMessage::setStreamFlag(int flag) {
	streamFlag = flag;
}

int BaseMessage::getType() {
	return header.type;
}

long BaseMessage::getOwnerAddress() {
	return header.ownerAddress;
}

void BaseMessage::setOwnerAddress(long address) {
	header.ownerAddress = address;
}

long BaseMessage::getTime() {
	return header.time;
}

long BaseMessage::getDeviceID() {
    return header.deviceID;
}

long BaseMessage::getMessageID() {
    return header.messageID;
}

long BaseMessage::getVariant(int id) {

    if (id < MAX_VARIANT) {
        return header.variant[id];
    }

    return 0;
}

void BaseMessage::setVariant(int id, long variant) {

    if (id >= MAX_VARIANT) {
        return;
    }

    header.variant[id] = variant;
}

PRIORITIES BaseMessage::getPriority() {
	return (PRIORITIES)header.priority;
}

PRIORITIES BaseMessage::iteratePriority() {

	if (header.priority > PRIORITY_1) {
		header.priority--;
	}

	return (PRIORITIES)header.priority;
}

void BaseMessage::setPriority(PRIORITIES priority) {
	header.priority = priority;
}

void BaseMessage::normalizePriority() {
	header.priority *= PRIORITY_COEFFICIENT;
}

int BaseMessage::getBinarySize(const std::string &path) {

    FILE *fd = fopen(path.c_str(), "rb");
    if (!fd) {
        LOG_E("File %s could not found", path.c_str());
        return 0;
    }

    fseek(fd, 0, SEEK_END);
    int fileSize = (int)ftell(fd);

    fclose(fd);

    return fileSize;
}

bool BaseMessage::transferBinary(int in, int out, uint8_t *md5, int size) {

    uint8_t buf[BUFFER_SIZE];

    MD5_CTX ctx;
    MD5_Init(&ctx);

    bool error = false;
    int readSize;

    do {

        if (size > BUFFER_SIZE) {
            readSize = BUFFER_SIZE;
            size -= readSize;
        } else {
            readSize = size;
            size = 0;
        }

        if (!readBlock(in, buf, (unsigned)readSize)) {
            LOG_E("Can not read data in transferBinary");
            error = true;
            break;
        }

        if (!writeBlock(out, buf, (unsigned)readSize)) {
            LOG_E("Can not write data in transferBinary");
            error = true;
            break;
        }

        MD5_Update(&ctx, buf, (unsigned)readSize);

    } while(size > 0);

    MD5_Final(md5, &ctx);

    return !error;

}

bool BaseMessage::readBlock(int in, uint8_t *buf, int size) {

	int offset = 0;
	bool busy = false;

	do {

		long count = read(in, buf + offset, (size_t)size);

		if (count == -1) {

			if (errno == EINTR) {
				LOG_W("Interrupt Block, retry");
				continue;
			}

			if (errno == EAGAIN && !busy) {
				LOG_W("Busy state, sleep and retry");
				busy = true;
				usleep(200000);
				continue;
			}

			LOG_E("Can not read data block");
			return false;
		}

		if (count == 0) {
			LOG_E("Empty read operation");
			return false;
		}

		if (count < size) {
			size -= count;
			offset += count;

			if (size == 0) {
				break;
			}

			busy = false;
			continue;
		}

		break;

	} while(true);

	return true;
}



bool BaseMessage::readSignature(int in) {

	uint8_t buf[2];

	if (!readBlock(in, buf, 2)) {
		LOG_E("Can not read correct signature from stream");
		return false;
	}

    if (buf[0] != (SIGNATURE & 0xFF) || buf[1] != (SIGNATURE >> 8)) {
        LOG_E("Can not read correct signature from stream");
        return false;
    }

	return true;
}

bool BaseMessage::readHeader(int in, MessageHeader *header) {

	if (!readBlock(in, tmpBuf, sizeof(MessageHeader))) {
		LOG_E("Can not read message header from stream");
		return false;
	}

	uint8_t *p = tmpBuf;
    header->type = *((int *) p); p += 4;
	header->priority = *((int *) p); p += 4;
    header->ownerAddress = *((long *) p); p += 8;
    header->time = *((long *) p); p += 8;
    header->deviceID = *((long *) p); p += 8;
    header->messageID = *((long *) p); p += 8;
    for (int i = 0; i < MAX_VARIANT; i++) {
        header->variant[i] = *((long *) p); p += 8;
    }

	return true;
}

bool BaseMessage::readBlockHeader(int in, BlockHeader *header) {

	if (!readBlock(in, tmpBuf, BLOCK_HEADER_SIZE)) {
		LOG_E("Can not read block header from stream");
		return false;
	}

	unsigned char *p = tmpBuf;
	header->blockType = *((int *) p); p += 4;
	header->blockCount = *((int *) p); p += 4;

    if (header->blockCount > 0) {

        header->sizes = new int[header->blockCount];

        if (!readBlock(in, tmpBuf, header->blockCount * 4)) {
            LOG_E("Can not read block header from stream");
            return false;
        }

        p = tmpBuf;
        for (int i = 0; i < header->blockCount; i++) {
            header->sizes[i] = *((int *) p); p += 4;
        }
    }

	return true;
}

bool BaseMessage::readString(int in, std::string &object, int size) {

	object.assign("");

	while(size > TMP_BUFFER_SIZE - 1) {

		if (!readBlock(in, tmpBuf, TMP_BUFFER_SIZE - 1)) {
			LOG_E("Can not read object data from stream");
			return false;
		}

        tmpBuf[TMP_BUFFER_SIZE - 1] = '\0';
		object.append((char *)tmpBuf);

		size -= TMP_BUFFER_SIZE + 1;
	}

	if (!readBlock(in, tmpBuf, size)) {
		LOG_E("Can not read object data from stream");
		return false;
	}

    tmpBuf[size] = '\0';
	object.append((char *)tmpBuf);

	return true;
}

bool BaseMessage::readNumber(int in, long *number) {
	//size must be 8;
	if (!readBlock(in, tmpBuf, 8)) {
		LOG_E("Can not read number from stream");
		return false;
	}

    number[0] = *((int *) tmpBuf);
	return true;
}

bool BaseMessage::readBinary(int in, const std::string &path, uint8_t *md5, int size) {

	Tools::mkpath(path.c_str());

	int out = open(path.c_str(), O_CREAT|O_WRONLY|O_TRUNC, 00755);
	if (out == -1) {
		LOG_E("File %s could not created or opened", path.c_str());
		return false;
	}

	bool status = transferBinary(in, out, md5, size);

	close(out);

	if (status) {

		std::string sMD5 = path + ".md5";

        out = open(sMD5.c_str(), O_CREAT|O_WRONLY|O_TRUNC, 00755);
        if (out == -1) {
            LOG_E("File %s could not created or opened", sMD5.c_str());
            return false;
        }

		if (!writeBlock(out, (const uint8_t*)Tools::getMD5Str(md5).c_str(), MD5_DIGEST_LENGTH * 2)) {
			LOG_E("Can not write md5 to file system in readBinary");
			status = false;
		}

		close(out);
	}

	return status;

}

bool BaseMessage::readFromStream(int in) {

	if (!readSignature(in)) {
		return false;
	}

	if (!readHeader(in, &header)) {
		return false;
	}

	struct BlockHeader blockHeader;

    do {

        if (!readBlockHeader(in, &blockHeader)) {
            return false;
        }

        if (blockHeader.blockType == BLOCK_END_STREAM) {
            return true;

        } else if (!readMessageBlock(in, &blockHeader)) {
            return false;
        }

    } while(true);
}

bool BaseMessage::writeBlock(int out, const uint8_t *buf, int size) {

	int offset = 0;
	bool busy = false;

	do {

		long count = write(out, buf + offset, (size_t)size);

		if (count == -1) {

			if (errno == EINTR) {
				LOG_W("Interrupt Block, retry");
				continue;
			}

			if (errno == EAGAIN && !busy) {
				LOG_W("Busy state, sleep and retry");
				busy = true;
				usleep(200000);
				continue;
			}

			LOG_E("Can not write data block");
			return false;
		}

		if (count == 0) {
			LOG_E("Empty write operation");
			return false;
		}

		if (count < size) {
			size -= count;
			offset += count;

			if (size == 0) {
				break;
			}

			busy = false;
			continue;
		}

		break;

	} while(true);

	return true;

}

bool BaseMessage::writeSignature(int out) {

    tmpBuf[0] = SIGNATURE & 0xFF;
    tmpBuf[1] = SIGNATURE >> 8;

    if (!writeBlock(out, tmpBuf, 2)) {
        LOG_E("Can not write signature to stream");
        return false;
    }

	return true;
}

bool BaseMessage::writeHeader(int out, MessageHeader *header) {

	if (!writeBlock(out, (uint8_t *)header, sizeof(MessageHeader))) {
		LOG_E("Can not write message header to stream");
		return false;
	}

	return true;
}

bool BaseMessage::writeBlockHeader(int out, BlockHeader *header) {

    uint8_t *p = tmpBuf;
    *((int *) p) = header->blockType; p += 4;
    *((int *) p) = header->blockCount; p += 4;
    for (int i = 0; i < header->blockCount; i++) {
        *((int *) p) = header->sizes[i]; p += 4;
    }

    if (!writeBlock(out, tmpBuf, BLOCK_HEADER_SIZE + header->blockCount * 4)) {
        LOG_E("Can not write block header to stream");
        return false;
    }

	return true;
}

bool BaseMessage::writeString(int out, const std::string &str) {

	if (!writeBlock(out, (uint8_t*)str.c_str(), (uint32_t)str.length())) {
		LOG_E("Can not write string to stream");
		return false;
	}

	return true;
}

bool BaseMessage::writeNumber(int out, long number) {

	if (!writeBlock(out, (uint8_t *)&number, 8)) {
		LOG_E("Can not write char array to stream");
		return false;
	}
	return true;
}

bool BaseMessage::writeBinary(int out, const std::string &path, uint8_t *md5) {

    int size = getBinarySize(path);

	int in = open(path.c_str(), O_RDONLY);
	if (in == -1) {
		LOG_E("File %s could not created or opened", path.c_str());
		return false;
	}

	bool status = transferBinary(in, out, md5, size);

	close(in);

	return status;

}

bool BaseMessage::writeEndStream(int out) {

	BlockHeader blockHeader = {0};

	return writeBlockHeader(out, &blockHeader);
}

bool BaseMessage::writeToStream(int out) {

	if (!writeSignature(out)) {
		return false;
	}

	if (!writeHeader(out, &header)) {
		return false;
	}

    if (streamFlag != STREAM_NONE) {

        if (!writeMessageStream(out, streamFlag)) {
            return false;
        }
    }

    writeEndStream(out);

	return true;
}
