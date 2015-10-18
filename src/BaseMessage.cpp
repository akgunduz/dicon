//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "BaseMessage.h"
#include "Util.h"

BaseMessage::BaseMessage(Unit host) {

	memset(&header, 0, sizeof(MessageHeader));
    setHost(host);
	setStreamFlag(STREAM_NONE);
	setPriority(DEFAULT_PRIORITY);
}

BaseMessage::BaseMessage(Unit owner, int type) {

	memset(&header, 0, sizeof(MessageHeader));

	header.time = 0;
	header.deviceID = 0;
	header.messageID = 0;//new Random(time).nextLong();
	header.type = type;

    setHost(owner);
    setOwner(owner);
	setStreamFlag(STREAM_NONE);
	setPriority(DEFAULT_PRIORITY);
}

void BaseMessage::setStreamFlag(int flag) {
	streamFlag = flag;
}

int BaseMessage::getType() {
	return header.type;
}

Unit BaseMessage::getHost() {
    return this->host;
}

void BaseMessage::setHost(Unit host) {
    this->host = host;
}

Unit BaseMessage::getOwner() {
	return Unit(header.owner);
}

void BaseMessage::setOwner(Unit owner) {
	header.owner = owner.getUnit();
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

int BaseMessage::getPriority() {
	return header.priority;
}

int BaseMessage::iteratePriority() {

	if (header.priority > 1) {
		header.priority--;
	}

	return header.priority;
}

void BaseMessage::setPriority(int priority) {
	header.priority = priority;
}

void BaseMessage::normalizePriority() {
	header.priority *= PRIORITY_COEFFICIENT;
}

int BaseMessage::getBinarySize(const char* path) {

    FILE *fd = fopen(path, "rb");
    if (!fd) {
        LOG_E("File %s could not found", path);
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

	if (!readBlock(in, tmpBuf, 2)) {
		LOG_E("Can not read correct signature from stream");
		return false;
	}

	short signature = ntohs(*((short *) tmpBuf));

    if (signature != SIGNATURE) {
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
    header->type = ntohl(*((int *) p)); p += 4;
	header->priority = ntohl(*((int *) p)); p += 4;
	header->owner = ntohl(*((int *) p)); p += 4;
    header->ownerAddress = ntohll(*((long *) p)); p += 8;
    header->time = ntohll(*((long *) p)); p += 8;
    header->deviceID = ntohll(*((long *) p)); p += 8;
    header->messageID = ntohll(*((long *) p)); p += 8;
    for (int i = 0; i < MAX_VARIANT; i++) {
        header->variant[i] = ntohll(*((long *) p)); p += 8;
    }

	return true;
}

bool BaseMessage::readBlockHeader(int in, BlockHeader *header) {

	if (!readBlock(in, tmpBuf, BLOCK_HEADER_SIZE)) {
		LOG_E("Can not read block header from stream");
		return false;
	}

	uint8_t *p = tmpBuf;
	header->blockType = ntohl(*((int *) p)); p += 4;
	header->blockCount = ntohl(*((int *) p)); p += 4;

    if (header->blockCount > 0) {

        header->sizes = new int[header->blockCount];

        if (!readBlock(in, tmpBuf, header->blockCount * 4)) {
            LOG_E("Can not read block header from stream");
            return false;
        }

        p = tmpBuf;
        for (int i = 0; i < header->blockCount; i++) {
            header->sizes[i] = ntohl(*((int *) p)); p += 4;
        }
    }

	return true;
}

bool BaseMessage::readString(int in, char* object, int size) {

    strcpy(object, "");

	while(size > TMP_BUFFER_SIZE - 1) {

		if (!readBlock(in, tmpBuf, TMP_BUFFER_SIZE - 1)) {
			LOG_E("Can not read object data from stream");
			return false;
		}

        tmpBuf[TMP_BUFFER_SIZE - 1] = '\0';
		strcat(object, (char *)tmpBuf);

		size -= TMP_BUFFER_SIZE + 1;
	}

	if (!readBlock(in, tmpBuf, size)) {
		LOG_E("Can not read object data from stream");
		return false;
	}

    tmpBuf[size] = '\0';
    strcat(object, (char *)tmpBuf);

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

bool BaseMessage::readBinary(int in, const char* path, uint8_t *md5, const char* md5Path, int size) {

	Util::mkPath(path);

	int out = open(path, O_CREAT|O_WRONLY|O_TRUNC, 00755);
	if (out == -1) {
		LOG_E("File %s could not created or opened", path);
		return false;
	}

	bool status = transferBinary(in, out, md5, size);

	close(out);

	if (status) {

        out = open(md5Path, O_CREAT|O_WRONLY|O_TRUNC, 00755);
        if (out == -1) {
            LOG_E("File %s could not created or opened", md5Path);
            return false;
        }

        const uint8_t* sMD5 = (const uint8_t*)Util::hex2str(md5, MD5_DIGEST_LENGTH).c_str();

		if (!writeBlock(out, sMD5, MD5_DIGEST_LENGTH * 2)) {
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

	*((short *) tmpBuf) = htons(SIGNATURE);

    if (!writeBlock(out, tmpBuf, 2)) {
        LOG_E("Can not write signature to stream");
        return false;
    }

	return true;
}

bool BaseMessage::writeHeader(int out, MessageHeader *header) {

	uint8_t *p = tmpBuf;
	*((int *) p) = htonl(header->type); p += 4;
	*((int *) p) = htonl(header->priority); p += 4;
	*((int *) p) = htonl(header->owner); p += 4;
	*((long *) p) = htonll(header->ownerAddress); p += 8;
	*((long *) p) = htonll(header->time); p += 8;
	*((long *) p) = htonll(header->deviceID); p += 8;
	*((long *) p) = htonll(header->messageID); p += 8;
    for (int i = 0; i < MAX_VARIANT; i++) {
        *((long *) p) = htonll(header->variant[i]); p += 8;
    }

	if (!writeBlock(out, tmpBuf, sizeof(MessageHeader))) {
		LOG_E("Can not write message header to stream");
		return false;
	}

	return true;
}

bool BaseMessage::writeBlockHeader(int out, BlockHeader *header) {

    uint8_t *p = tmpBuf;
    *((int *) p) = htonl(header->blockType); p += 4;
    *((int *) p) = htonl(header->blockCount); p += 4;
    for (int i = 0; i < header->blockCount; i++) {
        *((int *) p) = htonl(header->sizes[i]); p += 4;
    }

    if (!writeBlock(out, tmpBuf, BLOCK_HEADER_SIZE + header->blockCount * 4)) {
        LOG_E("Can not write block header to stream");
        return false;
    }

	return true;
}

bool BaseMessage::writeString(int out, const char* str) {

	if (!writeBlock(out, (uint8_t*)str, (int)strlen(str))) {
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

bool BaseMessage::writeBinary(int out, const char* path, uint8_t *md5) {

    int size = getBinarySize(path);

	int in = open(path, O_RDONLY);
	if (in == -1) {
		LOG_E("File %s could not created or opened", path);
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
