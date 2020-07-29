//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "MessageBase.h"
#include "Log.h"
#include "Util.h"
#include "Address.h"
#include "Net.h"
#include "UnixSocket.h"

CRC::Table<std::uint32_t, 32> MessageBase::crcTable{CRC::CRC_32()};

MessageBase::MessageBase(ComponentObject &host)
		: host(host) {
}

TypeReadCB MessageBase::getReadCB(long source) {

    if (Address::getInterface(source) == INTERFACE_NET) {

        return Net::getReadCB(source);
    }

    return UnixSocket::getReadCB(source);
}

TypeWriteCB MessageBase::getWriteCB(long source) {

    if (Address::getInterface(source) == INTERFACE_NET) {

        return Net::getWriteCB(source);
    }

    return UnixSocket::getWriteCB(source);
}

bool MessageBase::transferBinary(long source, long target, long size, uint32_t& crc) {

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

        if (!readBlock(source, buf, readSize, crc)) {
			LOGS_E(getHost(), "Can not read data in transferBinary");
            error = true;
            break;
        }

        if (!writeBlock(target, buf, readSize, fakeCrc)) {
			LOGS_E(getHost(), "Can not write data in transferBinary");
            error = true;
            break;
        }

    } while(size > 0);

    return !error;

}

bool MessageBase::readBlock(long source, uint8_t *buf, long size, uint32_t& crc) {

	long offset = 0;
	bool busy = false;

	do {

        long count = getReadCB(source)(source, buf + offset, (size_t) size);

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

bool MessageBase::readSignature(long source, uint32_t& crc) {

    LOGS_T(getHost(), "Signature read process is started");

	if (!readBlock(source, tmpBuf, SIGNATURE_SIZE, crc)) {
		LOGS_E(getHost(), "Can not read signature from stream");
		return false;
	}

	short signature = ntohs(*((short *) tmpBuf));

    if (signature != SIGNATURE) {
		LOGS_E(getHost(), "Signature Mismatch!!!, read : %x, expected : %x", signature, SIGNATURE);
        return false;
    }

    LOGS_T(getHost(), "Signature is read successfully");

	return true;
}

bool MessageBase::readHeader(long source, uint32_t& crc) {

    LOGS_T(getHost(), "Header read process is started");

	if (!readBlock(source, tmpBuf, getHeaderSize(), crc)) {
		LOGS_E(getHost(), "Can not read message header from stream");
		return false;
	}

    LOGS_T(getHost(), "Header is read successfully");

	return deSerializeHeader(tmpBuf);
}

bool MessageBase::readBlockHeader(long source, MessageBlockHeader &blockHeader, uint32_t& crc) {

    LOGS_T(getHost(), "Block Header read process is started");

	if (!readBlock(source, tmpBuf, BLOCK_HEADER_SIZE, crc)) {
		LOGS_E(getHost(), "Can not read block header from stream");
		return false;
	}

	uint8_t *p = tmpBuf;
    blockHeader.setType(ntohl(*((int *) p))); p += 4;
    int count = ntohl(*((int *) p));p += 4;

    if (count > 0) {

        if (!readBlock(source, tmpBuf, count * 8, crc)) {
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

bool MessageBase::readString(long source, char* str, long size, uint32_t& crc) {

    strcpy(str, "");

    LOGS_T(getHost(), "String read process is started => String Length: %d", size);

	while(size > TMP_BUFFER_SIZE - 1) {

		if (!readBlock(source, tmpBuf, TMP_BUFFER_SIZE - 1, crc)) {
			LOGS_E(getHost(), "Can not read object data from stream");
			return false;
		}

        tmpBuf[TMP_BUFFER_SIZE - 1] = '\0';
		strcat(str, (char *)tmpBuf);

		size -= TMP_BUFFER_SIZE + 1;
	}

	if (!readBlock(source, tmpBuf, size, crc)) {
		LOGS_E(getHost(), "Can not read object data from stream");
		return false;
	}

    tmpBuf[size] = '\0';
    strcat(str, (char *)tmpBuf);

    LOGS_T(getHost(), "String is read successfully => String : %s", str);

	return true;
}

bool MessageBase::readNumber(long source, long &number, uint32_t& crc) {

    LOGS_T(getHost(), "Number read process is started");

	if (!readBlock(source, tmpBuf, 64, crc)) {
		LOGS_E(getHost(), "Can not read number from stream");
		return false;
	}

    number = ntohll(*((long *) tmpBuf));

    LOGS_T(getHost(), "Number is read successfully, number => %ld", number);

	return true;
}

bool MessageBase::readNumberList(long source, std::vector<long> &list, long size, uint32_t& crc) {

    LOGS_T(getHost(), "NumberList read process is started, count : %ld", size);

    for (int i = 0; i < size; i++) {

        if (!readBlock(source, tmpBuf, 64, crc)) {
            LOGS_E(getHost(), "Can not read number from stream");
            return false;
        }

        list.emplace_back(ntohll(*((long *) tmpBuf)));
    }

    LOGS_T(getHost(), "NumberList is read successfully, count : %ld, first number => %ld", size, list[0]);

    return true;
}

bool MessageBase::readBinary(long source, const char* path, long size, uint32_t& crc) {

    LOGS_T(getHost(), "Read File is started at path : %s", path);

	Util::mkPath(path);

	int out = open(path, O_CREAT|O_WRONLY|O_TRUNC, 00755);
	if (out == -1) {
        LOGS_E(getHost(), "Read File could not created or opened at path : %s", path);
		return false;
	}

    long target = Address::create(INTERFACE_NET, 0, 0, out);

	bool status = transferBinary(source, target, size, crc);

	close(out);

    LOGS_T(getHost(), "File is read successfully => at path : %s", path);

	return status;

}

bool MessageBase::readCRC(long source, uint32_t &crc) {

    uint32_t resCrc;
    uint32_t calcCrc = crc;

    LOGS_T(getHost(), "CRC read process is started, => Calculated : %ud", calcCrc);

    if (!readBlock(source, tmpBuf, 32, crc)) {
        LOGS_E(getHost(), "Can not read number from stream");
        return false;
    }

    resCrc = ntohl(*((uint32_t *) tmpBuf));

    LOGS_T(getHost(), "CRC is read successfully, => Read : %ud, Calculated : %ud", resCrc, calcCrc);

    return resCrc == calcCrc;
}

bool MessageBase::readFromStream(long source) {

    uint32_t crc = 0;

	if (!readSignature(source, crc)) {
		return false;
	}

	if (!readHeader(source, crc)) {
		return false;
	}

    do {

        MessageBlockHeader blockHeader;

        if (!readBlockHeader(source, blockHeader, crc)) {
            return false;
        }

        if (blockHeader.isEnd()) {
            return readCRC(source, crc);

        } else if (!readMessageBlock(source, blockHeader, crc)) {
            return false;
        }

    } while(true);
}

bool MessageBase::writeBlock(long target, const uint8_t *buf, long size, uint32_t& crc) {

	long offset = 0;
	bool busy = false;

	do {

        long count = getWriteCB(target)(target, buf + offset, (size_t)size);

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

bool MessageBase::writeSignature(long target, uint32_t& crc) {

	*((short *) tmpBuf) = htons(SIGNATURE);

    LOGS_T(getHost(), "Signature write process is started");

    if (!writeBlock(target, tmpBuf, 2, crc)) {
        LOGS_E(getHost(), "Can not write signature to stream");
        return false;
    }

    LOGS_T(getHost(), "Signature is written successfully");

	return true;
}

bool MessageBase::writeHeader(long target, uint32_t& crc) {

    LOGS_T(getHost(), "Header write process is started");

    if (!serializeHeader(tmpBuf)) {
        LOGS_E(getHost(), "Can not prepare header");
        return false;
    }

	if (!writeBlock(target, tmpBuf, getHeaderSize(), crc)) {
		LOGS_E(getHost(), "Can not write message header to stream");
		return false;
	}

    LOGS_T(getHost(), "Header is written successfully");

	return true;
}

bool MessageBase::writeBlockHeader(long target, MessageBlockHeader &blockHeader, uint32_t& crc) {

    uint8_t *p = tmpBuf;
    *((int *) p) = htonl(blockHeader.getType()); p += 4;
    *((int *) p) = htonl(blockHeader.getCount()); p += 4;
    for (int i = 0; i < blockHeader.getCount(); i++) {
        *((long *) p) = htonll(blockHeader.get(i)); p += 8;
    }

    LOGS_T(getHost(), "Block header write process is started => Type : %d, Count : %d",
           blockHeader.getType(), blockHeader.getCount());

    if (!writeBlock(target, tmpBuf, p - tmpBuf, crc)) {
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

bool MessageBase::writeString(long target, const char* str, uint32_t& crc) {

    LOGS_T(getHost(), "String write process is started => String : %s", str);

	if (!writeBlock(target, (uint8_t*)str, strlen(str), crc)) {
		LOGS_E(getHost(), "Can not write string to stream");
		return false;
	}

    LOGS_T(getHost(), "String is written successfully => String : %s", str);

	return true;
}

bool MessageBase::writeNumber(long target, long number, uint32_t& crc) {

    uint8_t buffer[64];
    *((long *) buffer) = htonll(number);

    LOGS_T(getHost(), "Number write process is started => Number : %ld", number);

	if (!writeBlock(target, buffer, 64, crc)) {
		LOGS_E(getHost(), "Can not write char array to stream");
		return false;
	}

    LOGS_T(getHost(), "Number is written successfully => Number : %ld", number);

	return true;
}

bool MessageBase::writeNumberList(long target, std::vector<long>& list, uint32_t& crc) {

    uint8_t buffer[64];

    LOGS_T(getHost(), "NumberList write process is started  => Count : %ld, First Number : %ld",
           list.size(), list[0]);

    for (auto number : list) {

        *((long *) buffer) = htonll(number);
        if (!writeBlock(target, buffer, 64, crc)) {
            LOGS_E(getHost(), "Can not write char array to stream");
            return false;
        }
    }

    LOGS_T(getHost(), "NumberList is written successfully => Count : %ld, First Number : %ld",
            list.size(), list[0]);

    return true;
}

bool MessageBase::writeBinary(long target, const char* path, long size, uint32_t& crc) {

    LOGS_T(getHost(), "File Binary write process is started at path : %s", path);

	int in = open(path, O_RDONLY);
	if (in == -1) {
		LOGS_E(getHost(), "File Binary could not created or opened at path : %s", path);
		return false;
	}

	long source = Address::create(INTERFACE_NET, 0, 0, in);

	bool status = transferBinary(source, target, size, crc);

	close(in);

    LOGS_T(getHost(), "File Binary is written successfully at path : %s", path);

	return status;

}

bool MessageBase::writeEndStream(long target, uint32_t& crc) {

	MessageBlockHeader block;

    LOGS_T(getHost(), "End Stream write process is started");

    if (!writeBlockHeader(target, block, crc)) {

        LOGS_E(getHost(), "End Stream can not be written");
        return false;
    }

    LOGS_T(getHost(), "End Stream is written successfully");

	return true;
}

bool MessageBase::writeCRC(long target, uint32_t &crc) {

    uint8_t buffer[32];
    uint32_t calcCRC = crc;
    *((uint32_t *) buffer) = htonl(crc);

    LOGS_T(getHost(), "CRC write process is started => %d", calcCRC);

    if (!writeBlock(target, buffer, 32, crc)) {
        LOGS_E(getHost(), "Can not write CRC to stream");
        return false;
    }

    LOGS_T(getHost(), "CRC is written successfully => %d", calcCRC);

    return true;
}

bool MessageBase::writeToStream(long target) {

    uint32_t crc = 0;

	if (!writeSignature(target, crc)) {
		return false;
	}

	if (!writeHeader(target, crc)) {
		return false;
	}

    if (!writeMessageStream(target, crc)) {
        return false;
    }

    writeEndStream(target, crc);

	return writeCRC(target, crc);
}

ComponentObject& MessageBase::getHost() {

    return host;
}
