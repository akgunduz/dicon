//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "MessageBase.h"
#include "Log.h"
#include "Util.h"

CRC::Table<std::uint32_t, 32> MessageBase::crcTable{CRC::CRC_32()};

MessageBase::MessageBase(ComponentObject &host)
		: host(host) {
}

void MessageBase::setDatagramAddress(sockaddr_in address) {

    datagramAddress = address;
}

bool MessageBase::transferBinary(int in, int out, long size, uint32_t& crc) {

    uint8_t buf[BUFFER_SIZE];

    bool error = false;
    long readSize;
    uint32_t fakeCrc;

    do {

        if (size > BUFFER_SIZE) {
            readSize = BUFFER_SIZE;
            size -= readSize;
        } else {
            readSize = size;
            size = 0;
        }

        if (!readBlock(in, buf, readSize, crc)) {
			LOGS_E(getHost(), "Can not read data in transferBinary");
            error = true;
            break;
        }

        if (!writeBlock(out, buf, readSize, fakeCrc)) {
			LOGS_E(getHost(), "Can not write data in transferBinary");
            error = true;
            break;
        }

    } while(size > 0);

    return !error;

}

bool MessageBase::readBlock(int in, uint8_t *buf, long size, uint32_t& crc) {

	long offset = 0;
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
                std::this_thread::sleep_for(std::chrono::milliseconds(BUSY_SLEEP_TIME));
				continue;
			}

			LOGS_E(getHost(), "Can not read data block");
			return false;
		}

		if (count == 0) {
			LOGS_E(getHost(), "Empty read operation");
			return false;
		}

        crc = CRC::Calculate(buf + offset, count, crcTable, crc);

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



bool MessageBase::readSignature(int in, uint32_t& crc) {

    LOGS_T(getHost(), "Signature read process is started");

	if (!readBlock(in, tmpBuf, SIGNATURE_SIZE, crc)) {
		LOGS_E(getHost(), "Can not read correct signature from stream");
		return false;
	}

	short signature = ntohs(*((short *) tmpBuf));

    if (signature != SIGNATURE) {
		LOGS_E(getHost(), "Can not read correct signature from stream");
        return false;
    }

    LOGS_T(getHost(), "Signature is read successfully");

	return true;
}

bool MessageBase::readHeader(int in, uint32_t& crc) {

    LOGS_T(getHost(), "Header read process is started");

	if (!readBlock(in, tmpBuf, getHeaderSize(), crc)) {
		LOGS_E(getHost(), "Can not read message header from stream");
		return false;
	}

    LOGS_T(getHost(), "Header is read successfully");

	return deSerializeHeader(tmpBuf);
}

bool MessageBase::readBlockHeader(int in, MessageBlockHeader &blockHeader, uint32_t& crc) {

    LOGS_T(getHost(), "Block Header read process is started");

	if (!readBlock(in, tmpBuf, BLOCK_HEADER_SIZE, crc)) {
		LOGS_E(getHost(), "Can not read block header from stream");
		return false;
	}

	uint8_t *p = tmpBuf;
    blockHeader.setType(ntohl(*((int *) p))); p += 4;
    int count = ntohl(*((int *) p));p += 4;

    if (count > 0) {

        if (!readBlock(in, tmpBuf, count * 8, crc)) {
            LOGS_E(getHost(), "Can not read block header from stream");
            return false;
        }

        p = tmpBuf;
        for (int i = 0; i < count; i++) {
            blockHeader.add(ntohll(*((long *) p))); p += 8;
        }
    }

    LOGS_T(getHost(), "Block Header is read successfully => Type : %s, Count : %d",
           MessageTypes::getBlockName(blockHeader.getType()), blockHeader.getCount());

    for (int i = 0; i < blockHeader.getCount(); i++) {
        LOGS_T(getHost(), "Block Header Data[%d] : %d", i, blockHeader.get(i));
    }

	return true;
}

bool MessageBase::readString(int in, char* str, long size, uint32_t& crc) {

    strcpy(str, "");

    LOGS_T(getHost(), "String read process is started => String Length: %d", size);

	while(size > TMP_BUFFER_SIZE - 1) {

		if (!readBlock(in, tmpBuf, TMP_BUFFER_SIZE - 1, crc)) {
			LOGS_E(getHost(), "Can not read object data from stream");
			return false;
		}

        tmpBuf[TMP_BUFFER_SIZE - 1] = '\0';
		strcat(str, (char *)tmpBuf);

		size -= TMP_BUFFER_SIZE + 1;
	}

	if (!readBlock(in, tmpBuf, size, crc)) {
		LOGS_E(getHost(), "Can not read object data from stream");
		return false;
	}

    tmpBuf[size] = '\0';
    strcat(str, (char *)tmpBuf);

    LOGS_T(getHost(), "String is read successfully => String : %s", str);

	return true;
}

bool MessageBase::readNumber(int in, long &number, uint32_t& crc) {

    LOGS_T(getHost(), "Number read process is started");

	if (!readBlock(in, tmpBuf, 64, crc)) {
		LOGS_E(getHost(), "Can not read number from stream");
		return false;
	}

    number = ntohll(*((long *) tmpBuf));

    LOGS_T(getHost(), "Number is read successfully, number => %ld", number);

	return true;
}

bool MessageBase::readNumberList(int in, std::vector<long> &list, long size, uint32_t& crc) {

    LOGS_T(getHost(), "NumberList read process is started, count : %ld", size);

    for (int i = 0; i < size; i++) {

        if (!readBlock(in, tmpBuf, 64, crc)) {
            LOGS_E(getHost(), "Can not read number from stream");
            return false;
        }

        list.emplace_back(ntohll(*((long *) tmpBuf)));
    }

    LOGS_T(getHost(), "NumberList is read successfully, count : %ld, first number => %ld", size, list[0]);

    return true;
}

bool MessageBase::readBinary(int in, const char* path, long size, uint32_t& crc) {

    LOGS_T(getHost(), "Read File is started at path : %s", path);

	Util::mkPath(path);

	int out = open(path, O_CREAT|O_WRONLY|O_TRUNC, 00755);
	if (out == -1) {
        LOGS_E(getHost(), "Read File could not created or opened at path : %s", path);
		return false;
	}

	bool status = transferBinary(in, out, size, crc);

	close(out);

    LOGS_T(getHost(), "File is read successfully => at path : %s", path);

	return status;

}

bool MessageBase::readCRC(int in, uint32_t &crc) {

    uint32_t resCrc;
    uint32_t calcCrc = crc;

    LOGS_T(getHost(), "CRC read process is started, => Calculated : %ud", calcCrc);

    if (!readBlock(in, tmpBuf, 32, crc)) {
        LOGS_E(getHost(), "Can not read number from stream");
        return false;
    }

    resCrc = ntohl(*((uint32_t *) tmpBuf));

    LOGS_T(getHost(), "CRC is read successfully, => Read : %ud, Calculated : %ud", resCrc, calcCrc);

    return resCrc == calcCrc;
}

bool MessageBase::readFromStream(int in) {

    uint32_t crc = 0;

	if (!readSignature(in, crc)) {
		return false;
	}

	if (!readHeader(in, crc)) {
		return false;
	}

    do {

        MessageBlockHeader blockHeader;

        if (!readBlockHeader(in, blockHeader, crc)) {
            return false;
        }

        if (blockHeader.isEnd()) {
            return readCRC(in, crc);

        } else if (!readMessageBlock(in, blockHeader, crc)) {
            return false;
        }

    } while(true);
}

bool MessageBase::writeBlock(int out, const uint8_t *buf, long size, uint32_t& crc) {

	long offset = 0;
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
                std::this_thread::sleep_for(std::chrono::milliseconds(BUSY_SLEEP_TIME));
				continue;
			}

			LOGS_E(getHost(), "Can not write data block");
			return false;
		}

		if (count == 0) {
			LOGS_E(getHost(), "Empty write operation");
			return false;
		}

        crc = CRC::Calculate(buf + offset, count, crcTable, crc);

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

bool MessageBase::writeSignature(int out, uint32_t& crc) {

	*((short *) tmpBuf) = htons(SIGNATURE);

    LOGS_T(getHost(), "Signature write process is started");

    if (!writeBlock(out, tmpBuf, 2, crc)) {
        LOGS_E(getHost(), "Can not write signature to stream");
        return false;
    }

    LOGS_T(getHost(), "Signature is written successfully");

	return true;
}

bool MessageBase::writeHeader(int out, uint32_t& crc) {

    LOGS_T(getHost(), "Header write process is started");

    if (!serializeHeader(tmpBuf)) {
        LOGS_E(getHost(), "Can not prepare header");
        return false;
    }

	if (!writeBlock(out, tmpBuf, getHeaderSize(), crc)) {
		LOGS_E(getHost(), "Can not write message header to stream");
		return false;
	}

    LOGS_T(getHost(), "Header is written successfully");

	return true;
}

bool MessageBase::writeBlockHeader(int out, MessageBlockHeader &blockHeader, uint32_t& crc) {

    uint8_t *p = tmpBuf;
    *((int *) p) = htonl(blockHeader.getType()); p += 4;
    *((int *) p) = htonl(blockHeader.getCount()); p += 4;
    for (int i = 0; i < blockHeader.getCount(); i++) {
        *((long *) p) = htonll(blockHeader.get(i)); p += 8;
    }

    LOGS_T(getHost(), "Block header write process is started => Type : %d, Count : %d",
           blockHeader.getType(), blockHeader.getCount());

    if (!writeBlock(out, tmpBuf, p - tmpBuf, crc)) {
        LOGS_E(getHost(), "Can not write block header to stream");
        return false;
    }

    LOGS_T(getHost(), "Block header is written successfully => Type : %d, Count : %d",
           blockHeader.getType(), blockHeader.getCount());

    for (int i = 0; i < blockHeader.getCount(); i++) {
        LOGS_T(getHost(), "Block Header Data[%d] : %d", i, blockHeader.get(i));
    }

	return true;
}

bool MessageBase::writeString(int out, const char* str, uint32_t& crc) {

    LOGS_T(getHost(), "String write process is started => String : %s", str);

	if (!writeBlock(out, (uint8_t*)str, strlen(str), crc)) {
		LOGS_E(getHost(), "Can not write string to stream");
		return false;
	}

    LOGS_T(getHost(), "String is written successfully => String : %s", str);

	return true;
}

bool MessageBase::writeNumber(int out, long number, uint32_t& crc) {

    uint8_t buffer[64];
    *((long *) buffer) = htonll(number);

    LOGS_T(getHost(), "Number write process is started => Number : %ld", number);

	if (!writeBlock(out, buffer, 64, crc)) {
		LOGS_E(getHost(), "Can not write char array to stream");
		return false;
	}

    LOGS_T(getHost(), "Number is written successfully => Number : %ld", number);

	return true;
}

bool MessageBase::writeNumberList(int out, std::vector<long>& list, uint32_t& crc) {

    uint8_t buffer[64];

    LOGS_T(getHost(), "NumberList write process is started  => Count : %ld, First Number : %ld",
           list.size(), list[0]);

    for (auto number : list) {

        *((long *) buffer) = htonll(number);
        if (!writeBlock(out, buffer, 64, crc)) {
            LOGS_E(getHost(), "Can not write char array to stream");
            return false;
        }
    }

    LOGS_T(getHost(), "NumberList is written successfully => Count : %ld, First Number : %ld",
            list.size(), list[0]);

    return true;
}

bool MessageBase::writeBinary(int out, const char* path, long size, uint32_t& crc) {

    LOGS_T(getHost(), "File Binary write process is started at path : %s", path);

	int in = open(path, O_RDONLY);
	if (in == -1) {
		LOGS_E(getHost(), "File Binary could not created or opened at path : %s", path);
		return false;
	}

	bool status = transferBinary(in, out, size, crc);

	close(in);

    LOGS_T(getHost(), "File Binary is written successfully at path : %s", path);

	return status;

}

bool MessageBase::writeEndStream(int out, uint32_t& crc) {

	MessageBlockHeader block;

    LOGS_T(getHost(), "End Stream write process is started");

    if (!writeBlockHeader(out, block, crc)) {

        LOGS_E(getHost(), "End Stream can not be written");
        return false;
    }

    LOGS_T(getHost(), "End Stream is written successfully");

	return true;
}

bool MessageBase::writeCRC(int out, uint32_t &crc) {

    uint8_t buffer[32];
    uint32_t calcCRC = crc;
    *((uint32_t *) buffer) = htonl(crc);

    LOGS_T(getHost(), "CRC write process is started => %d", calcCRC);

    if (!writeBlock(out, buffer, 32, crc)) {
        LOGS_E(getHost(), "Can not write CRC to stream");
        return false;
    }

    LOGS_T(getHost(), "CRC is written successfully => %d", calcCRC);

    return true;
}

bool MessageBase::writeToStream(int out) {

    uint32_t crc = 0;

	if (!writeSignature(out, crc)) {
		return false;
	}

	if (!writeHeader(out, crc)) {
		return false;
	}

    if (!writeMessageStream(out, crc)) {
        return false;
    }

    writeEndStream(out, crc);

	return writeCRC(out, crc);
}

ComponentObject& MessageBase::getHost() {

    return host;
}
