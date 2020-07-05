//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "BaseMessage.h"
#include "AddressHelper.h"

BaseMessage::BaseMessage(ComponentObject host)
		: host(host) {

    bzero(&datagramAddress, sizeof(sockaddr_in));
}

void BaseMessage::setDatagramAddress(sockaddr_in address) {

    datagramAddress = address;
}

int BaseMessage::getBinarySize(const char* path) {

    FILE *fd = fopen(path, "rb");
    if (!fd) {
		LOGS_E(getHost(), "File %s could not found", path);
        return 0;
    }

    fseek(fd, 0, SEEK_END);
    int fileSize = (int)ftell(fd);

    fclose(fd);

    return fileSize;
}

bool BaseMessage::transferBinary(int in, int out, Md5 *md5, int size) {

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
			LOGS_E(getHost(), "Can not read data in transferBinary");
            error = true;
            break;
        }

        if (!writeBlock(out, buf, (unsigned)readSize)) {
			LOGS_E(getHost(), "Can not write data in transferBinary");
            error = true;
            break;
        }

        MD5_Update(&ctx, buf, (unsigned)readSize);

    } while(size > 0);

    if (md5 != NULL) {

        MD5_Final(md5->data, &ctx);
    }

    return !error;

}

bool BaseMessage::readBlock(int in, uint8_t *buf, int size) {

	int offset = 0;
	bool busy = false;

	do {
        long count;

        if (datagramAddress.sin_addr.s_addr == 0) {
            count = read(in, buf + offset, (size_t) size);

        } else {
            count = recvfrom(in, buf + offset, (size_t) size, 0, nullptr, nullptr);
        }

		if (count == -1) {

			if (errno == EINTR) {
				LOGS_W(getHost(), "Interrupt Block, retry");
				continue;
			}

			if (errno == EAGAIN && !busy) {
				LOGS_E(getHost(), "Busy state, sleep and retry");
				busy = true;
				usleep(BUSY_SLEEP_TIME);
				continue;
			}

			LOGS_E(getHost(), "Can not read data block");
			return false;
		}

		if (count == 0) {
			LOGS_E(getHost(), "Empty read operation");
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

	if (!readBlock(in, tmpBuf, SIGNATURE_SIZE)) {
		LOGS_E(getHost(), "Can not read correct signature from stream");
		return false;
	}

	short signature = ntohs(*((short *) tmpBuf));

    if (signature != SIGNATURE) {
		LOGS_E(getHost(), "Can not read correct signature from stream");
        return false;
    }

	return true;
}

bool BaseMessage::readHeader(int in) {

	if (!readBlock(in, tmpBuf, getHeaderSize())) {
		LOGS_E(getHost(), "Can not read message header from stream");
		return false;
	}

	return deSerializeHeader(tmpBuf);
}

bool BaseMessage::readBlockHeader(int in, Block *header) {

	if (!readBlock(in, tmpBuf, BLOCK_HEADER_SIZE)) {
		LOGS_E(getHost(), "Can not read block header from stream");
		return false;
	}

	uint8_t *p = tmpBuf;
    header->setType(ntohl(*((int *) p))); p += 4;
    header->setCount(ntohl(*((int *) p)));p += 4;

    if (header->getCount() > 0) {

        if (!readBlock(in, tmpBuf, header->getCount() * 4)) {
            LOGS_E(getHost(), "Can not read block header from stream");
            return false;
        }

        p = tmpBuf;
        for (int i = 0; i < header->getCount(); i++) {
            header->setSize(i, ntohl(*((int *) p))); p += 4;
        }
    }

	return true;
}

bool BaseMessage::readString(int in, char* object, int size) {

    strcpy(object, "");

	while(size > TMP_BUFFER_SIZE - 1) {

		if (!readBlock(in, tmpBuf, TMP_BUFFER_SIZE - 1)) {
			LOGS_E(getHost(), "Can not read object data from stream");
			return false;
		}

        tmpBuf[TMP_BUFFER_SIZE - 1] = '\0';
		strcat(object, (char *)tmpBuf);

		size -= TMP_BUFFER_SIZE + 1;
	}

	if (!readBlock(in, tmpBuf, size)) {
		LOGS_E(getHost(), "Can not read object data from stream");
		return false;
	}

    tmpBuf[size] = '\0';
    strcat(object, (char *)tmpBuf);

	return true;
}

bool BaseMessage::readNumber(int in, long *number) {
	//size must be 64;
	if (!readBlock(in, tmpBuf, 64)) {
		LOGS_E(getHost(), "Can not read number from stream");
		return false;
	}

    number[0] = ntohll(*((long *) tmpBuf));
	return true;
}

bool BaseMessage::readNumberList(int in, std::vector<long> &list, int size) {
    //size must be 64;

    for (int i = 0; i < size; i++) {

        if (!readBlock(in, tmpBuf, 64)) {
            LOGS_E(getHost(), "Can not read number from stream");
            return false;
        }

        list.emplace_back(ntohll(*((long *) tmpBuf)));
    }

    return true;
}


bool BaseMessage::readMD5(int desc, Md5* md5) {

	if (!readBlock(desc, md5->data, MD5_DIGEST_LENGTH)) {
		LOGS_E(getHost(), "Can not read MD5 from stream");
		return false;
	}

	return true;
}

bool BaseMessage::readBinary(int in, const char* path, Md5 *md5, int size) {

	Util::mkPath(path);

	int out = open(path, O_CREAT|O_WRONLY|O_TRUNC, 00755);
	if (out == -1) {
		LOGS_E(getHost(), "File %s could not created or opened", path);
		return false;
	}

	bool status = transferBinary(in, out, md5, size);

	close(out);

	return status;

}

bool BaseMessage::readFromStream(int in) {

	if (!readSignature(in)) {
		return false;
	}

	if (!readHeader(in)) {
		return false;
	}

	Block header;

    do {

        if (!readBlockHeader(in, &header)) {
            return false;
        }

        if (header.isEnd()) {
            return readFinalize();

        } else if (!readMessageBlock(in, &header)) {
            return false;
        }

    } while(true);
}

bool BaseMessage::writeBlock(int out, const uint8_t *buf, int size) {

	int offset = 0;
	bool busy = false;

	do {

        long count;

        if (datagramAddress.sin_addr.s_addr == 0) {
            count = write(out, buf + offset, (size_t)size);

        } else {
            count = sendto(out, buf + offset, (size_t)size, 0,
                           (struct sockaddr *) &datagramAddress, sizeof(struct sockaddr));
        }

		if (count == -1) {

			if (errno == EINTR) {
				LOGS_W(getHost(), "Interrupt Block, retry");
				continue;
			}

			if (errno == EAGAIN && !busy) {
				LOGS_W(getHost(), "Busy state, sleep and retry");
				busy = true;
				usleep(BUSY_SLEEP_TIME);
				continue;
			}

			LOGS_E(getHost(), "Can not write data block");
			return false;
		}

		if (count == 0) {
			LOGS_E(getHost(), "Empty write operation");
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
        LOGS_E(getHost(), "Can not write signature to stream");
        return false;
    }

	return true;
}

bool BaseMessage::writeHeader(int out) {

    if (!serializeHeader(tmpBuf)) {
        LOGS_E(getHost(), "Can not prepare header");
        return false;
    }

	if (!writeBlock(out, tmpBuf, getHeaderSize())) {
		LOGS_E(getHost(), "Can not write message header to stream");
		return false;
	}

	return true;
}

bool BaseMessage::writeBlockHeader(int out, Block *header) {

    uint8_t *p = tmpBuf;
    *((int *) p) = htonl(header->getType()); p += 4;
    *((int *) p) = htonl(header->getCount()); p += 4;
    for (int i = 0; i < header->getCount(); i++) {
        *((int *) p) = htonl(header->getSize(i)); p += 4;
    }

    if (!writeBlock(out, tmpBuf, BLOCK_HEADER_SIZE + header->getCount() * 4)) {
        LOGS_E(getHost(), "Can not write block header to stream");
        return false;
    }

	return true;
}

bool BaseMessage::writeString(int out, const char* str) {

	if (!writeBlock(out, (uint8_t*)str, (int)strlen(str))) {
		LOGS_E(getHost(), "Can not write string to stream");
		return false;
	}

	return true;
}

bool BaseMessage::writeNumber(int out, long number) {

    uint8_t buffer[64];
    *((long *) buffer) = htonll(number);

	if (!writeBlock(out, buffer, 64)) {
		LOGS_E(getHost(), "Can not write char array to stream");
		return false;
	}
	return true;
}

bool BaseMessage::writeNumberList(int out, std::vector<long>& list) {

    uint8_t buffer[64];

    for (auto number : list) {

        *((long *) buffer) = htonll(number);
        if (!writeBlock(out, buffer, 64)) {
            LOGS_E(getHost(), "Can not write char array to stream");
            return false;
        }
    }

    return true;
}

bool BaseMessage::writeMD5(int desc, Md5* md5) {

    if (!writeBlock(desc, md5->data, MD5_DIGEST_LENGTH)) {
        LOGS_E(getHost(), "Can not write md5 to stream");
        return false;
    }

    return true;
}


bool BaseMessage::writeBinary(int out, const char* path, Md5 *md5, int size) {

	int in = open(path, O_RDONLY);
	if (in == -1) {
		LOGS_E(getHost(), "File %s could not created or opened", path);
		return false;
	}

	bool status = transferBinary(in, out, md5, size);

	close(in);

	return status;

}

bool BaseMessage::writeEndStream(int out) {

	Block block(0);

	return writeBlockHeader(out, &block);
}

bool BaseMessage::writeToStream(int out) {

	if (!writeSignature(out)) {
		return false;
	}

	if (!writeHeader(out)) {
		return false;
	}

    if (!writeMessageStream(out)) {
        return false;
    }

    writeEndStream(out);

	return writeFinalize();
}

ComponentObject BaseMessage::getHost() {

    return this->host;
}
