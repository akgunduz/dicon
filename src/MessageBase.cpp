//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "MessageBase.h"
#include "Log.h"
#include "Util.h"
#include "Net.h"
#include "UnixSocket.h"

CRC::Table<std::uint32_t, 32> MessageBase::crcTable{CRC::CRC_32()};

MessageBase::MessageBase(const TypeHostUnit& host)
		: host(host) {
}

TypeReadCB MessageBase::getReadCB(const TypeComponentUnit& source) {

    if (source->getAddress().getInterface() == INTERFACE_NET) {

        return Net::getReadCB(source);
    }

    return UnixSocket::getReadCB(source);
}

TypeWriteCB MessageBase::getWriteCB(const TypeComponentUnit&  target) {

    if (target->getAddress().getInterface() == INTERFACE_NET) {

        return Net::getWriteCB(target);
    }

    return UnixSocket::getWriteCB(target);
}

bool MessageBase::transferBinary(const TypeComponentUnit& source, const TypeComponentUnit& target, size_t size, uint32_t& crc) {

    uint8_t buf[BUFFER_SIZE] = {};

    bool error = false;
    long readSize = 0;
    uint32_t fakeCrc = 0;

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

bool MessageBase::readBlock(const TypeComponentUnit& source, uint8_t *buf, size_t size, uint32_t& crc) {

	long offset = 0;
	bool busy = false;

	do {

        long count = getReadCB(source)(source, buf + offset, (size_t) size);

		if (count == -1) {

			if (errno == EINTR) {
				LOGC_W(getHost(), source, MSGDIR_RECEIVE, "Interrupt Block, retry");
				continue;
			}

			if (errno == EAGAIN && !busy) {
				LOGC_E(getHost(), source, MSGDIR_RECEIVE, "Busy state, sleep and retry");
				busy = true;
                std::this_thread::sleep_for(std::chrono::milliseconds(BUSY_SLEEP_TIME));
				continue;
			}

			LOGC_E(getHost(), source, MSGDIR_RECEIVE, "Can not read data block");
			return false;
		}

		if (count == 0) {
			LOGC_E(getHost(), source, MSGDIR_RECEIVE, "Empty read operation");
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

bool MessageBase::readSignature(const TypeComponentUnit& source, uint32_t& crc) {

    LOGS_T(getHost(), "Signature read process is started");

	if (!readBlock(source, tmpBuf, SIGNATURE_SIZE, crc)) {
		LOGS_E(getHost(), "Can not read signature from stream");
		return false;
	}

	short signature = ntohs(*((short *) tmpBuf));

    if (signature != SIGNATURE) {
		LOGS_E(getHost(), "Signature Mismatch!!!, read : 0x%X, expected : 0x%X", signature, SIGNATURE);
        return false;
    }

    LOGS_T(getHost(), "Signature is read successfully");

	return true;
}

bool MessageBase::readHeader(const TypeComponentUnit& source, uint32_t& crc) {

    LOGS_T(getHost(), "Header read process is started");

	if (!readBlock(source, tmpBuf, getHeaderSize(), crc)) {
		LOGS_E(getHost(), "Can not read message header from stream");
		return false;
	}

    deSerializeHeader(tmpBuf);

    LOGS_T(getHost(), "Header is read successfully");

	return true;
}

bool MessageBase::readBlockHeader(const TypeComponentUnit& source, MessageBlockHeader &blockHeader, uint32_t& crc) {

    LOGC_T(getHost(), source, MSGDIR_RECEIVE, "Block Header read process is started");

	if (!readBlock(source, tmpBuf, BLOCK_HEADER_SIZE, crc)) {
		LOGC_E(getHost(), source, MSGDIR_RECEIVE, "Can not read block header from stream");
		return false;
	}

	uint8_t *p = tmpBuf;
    blockHeader.setType(ntohl(*((int *) p))); p += 4;
    int count = ntohl(*((int *) p));p += 4;

    if (count > 0) {

        if (!readBlock(source, tmpBuf, count * 8, crc)) {
            LOGC_E(getHost(), source, MSGDIR_RECEIVE, "Can not read block header from stream");
            return false;
        }

        p = tmpBuf;
        for (int i = 0; i < count; i++) {
            blockHeader.add(ntohll(*((long *) p))); p += 8;
        }
    }

    LOGC_T(getHost(), source, MSGDIR_RECEIVE, "Block Header is read successfully => Type : %s, Count : %d",
           MessageType::getBlockName(blockHeader.getType()), blockHeader.getCount());

    for (int i = 0; i < blockHeader.getCount(); i++) {
        LOGC_T(getHost(), source, MSGDIR_RECEIVE, "Block Header Data[%d] : %d", i, blockHeader.get(i));
    }

	return true;
}

bool MessageBase::readString(const TypeComponentUnit& source, std::string& str, size_t size, uint32_t& crc) {

    LOGC_T(getHost(), source, MSGDIR_RECEIVE, "String read process is started => String Length: %d", size);

    str = "";

	while(size > TMP_BUFFER_SIZE - 1) {

		if (!readBlock(source, tmpBuf, TMP_BUFFER_SIZE - 1, crc)) {
			LOGC_E(getHost(), source, MSGDIR_RECEIVE, "Can not read object data from stream");
			return false;
		}

        tmpBuf[TMP_BUFFER_SIZE - 1] = '\0';
		str += (char *)tmpBuf;

		size -= TMP_BUFFER_SIZE + 1;
	}

	if (!readBlock(source, tmpBuf, size, crc)) {
		LOGC_E(getHost(), source, MSGDIR_RECEIVE, "Can not read object data from stream");
		return false;
	}

    tmpBuf[size] = '\0';
    str += (char *)tmpBuf;

    LOGC_T(getHost(), source, MSGDIR_RECEIVE, "String is read successfully => String : %s", str.c_str());

	return true;
}

bool MessageBase::readNumber(const TypeComponentUnit& source, long &number, uint32_t& crc) {

    LOGC_T(getHost(), source, MSGDIR_RECEIVE, "Number read process is started");

	if (!readBlock(source, tmpBuf, 64, crc)) {
		LOGC_E(getHost(), source, MSGDIR_RECEIVE, "Can not read number from stream");
		return false;
	}

    number = ntohll(*((long *) tmpBuf));

    LOGC_T(getHost(), source, MSGDIR_RECEIVE, "Number is read successfully, number => %ld", number);

	return true;
}

bool MessageBase::readNumberList(const TypeComponentUnit& source, std::vector<long> &list, size_t size, uint32_t& crc) {

    LOGC_T(getHost(), source, MSGDIR_RECEIVE, "NumberList read process is started, count : %ld", size);

    for (int i = 0; i < size; i++) {

        if (!readBlock(source, tmpBuf, 64, crc)) {
            LOGC_E(getHost(), source, MSGDIR_RECEIVE, "Can not read number from stream");
            return false;
        }

        list.emplace_back(ntohll(*((long *) tmpBuf)));
    }

    LOGC_T(getHost(), source, MSGDIR_RECEIVE, "NumberList is read successfully, count : %ld, first number => %ld", size, list[0]);

    return true;
}

bool MessageBase::readBinary(const TypeComponentUnit& source, const std::filesystem::path& path, size_t size, uint32_t& crc) {

    LOGC_T(getHost(), source, MSGDIR_RECEIVE, "Read File is started at path : %s", path.c_str());

    std::filesystem::create_directories(path.parent_path());

	int out = open(path.c_str(), O_CREAT|O_WRONLY|O_TRUNC, 00755);
	if (out == -1) {
        LOGC_E(getHost(), source, MSGDIR_RECEIVE, "Read File could not created or opened at path : %s", path.c_str());
		return false;
	}

    auto target = std::make_shared<ComponentUnit>(out);

	bool status = transferBinary(source, target, size, crc);

	close(out);

    LOGC_T(getHost(), source, MSGDIR_RECEIVE, "File is read successfully => at path : %s", path.c_str());

	return status;

}

bool MessageBase::readCRC(const TypeComponentUnit& source, uint32_t &crc) {

    uint32_t resCrc;
    uint32_t calcCrc = crc;

    LOGC_T(getHost(), source, MSGDIR_RECEIVE, "CRC read process is started, => Calculated : 0x%X", calcCrc);

    if (!readBlock(source, tmpBuf, 32, crc)) {
        LOGC_E(getHost(), source, MSGDIR_RECEIVE, "Can not read number from stream");
        return false;
    }

    resCrc = ntohl(*((uint32_t *) tmpBuf));

    LOGC_T(getHost(), source, MSGDIR_RECEIVE, "CRC is read successfully, => Read : 0x%X, Calculated : 0x%X", resCrc, calcCrc);

    return resCrc == calcCrc;
}

bool MessageBase::readFromStream(const TypeComponentUnit& source) {

    uint32_t crc = 0;

	if (!readSignature(source, crc)) {
		return false;
	}

	if (!readHeader(source, crc)) {
		return false;
	}

	grabOwner(source);

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

bool MessageBase::writeBlock(const TypeComponentUnit& target, const uint8_t *buf, size_t size, uint32_t& crc) {

	long offset = 0;
	bool busy = false;

	do {

        long count = getWriteCB(target)(target, buf + offset, (size_t)size);

		if (count == -1) {

			if (errno == EINTR) {
				LOGC_W(getHost(), target, MSGDIR_SEND, "Interrupt Block, retry");
				continue;
			}

			if (errno == EAGAIN && !busy) {
				LOGC_W(getHost(), target, MSGDIR_SEND, "Busy state, sleep and retry");
				busy = true;
                std::this_thread::sleep_for(std::chrono::milliseconds(BUSY_SLEEP_TIME));
				continue;
			}

			LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write data block");
			return false;
		}

		if (count == 0) {
			LOGC_E(getHost(), target, MSGDIR_SEND, "Empty write operation");
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

bool MessageBase::writeSignature(const TypeComponentUnit& target, uint32_t& crc) {

	*((short *) tmpBuf) = htons(SIGNATURE);

    LOGC_T(getHost(), target, MSGDIR_SEND, "Signature write process is started");

    if (!writeBlock(target, tmpBuf, 2, crc)) {
        LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write signature to stream");
        return false;
    }

    LOGC_T(getHost(), target, MSGDIR_SEND, "Signature is written successfully");

	return true;
}

bool MessageBase::writeHeader(const TypeComponentUnit& target, uint32_t& crc) {

    LOGC_T(getHost(), target, MSGDIR_SEND, "Header write process is started");

    serializeHeader(tmpBuf);

	if (!writeBlock(target, tmpBuf, getHeaderSize(), crc)) {
		LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write message header to stream");
		return false;
	}

    LOGC_T(getHost(), target, MSGDIR_SEND, "Header is written successfully");

	return true;
}

bool MessageBase::writeBlockHeader(const TypeComponentUnit& target, MessageBlockHeader &blockHeader, uint32_t& crc) {

    uint8_t *p = tmpBuf;
    *((int *) p) = htonl(blockHeader.getType()); p += 4;
    *((int *) p) = htonl(blockHeader.getCount()); p += 4;
    for (int i = 0; i < blockHeader.getCount(); i++) {
        *((long *) p) = htonll(blockHeader.get(i)); p += 8;
    }

    LOGC_T(getHost(), target, MSGDIR_SEND, "Block header write process is started => Type : %s, Count : %d",
           MessageType::getBlockName(blockHeader.getType()), blockHeader.getCount());

    if (!writeBlock(target, tmpBuf, p - tmpBuf, crc)) {
        LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write block header to stream");
        return false;
    }

    LOGC_T(getHost(), target, MSGDIR_SEND, "Block header is written successfully => Type : %s, Count : %d",
           MessageType::getBlockName(blockHeader.getType()), blockHeader.getCount());

    for (int i = 0; i < blockHeader.getCount(); i++) {
        LOGC_T(getHost(), target, MSGDIR_SEND, "Block Header Data[%d] : %d", i, blockHeader.get(i));
    }

	return true;
}

bool MessageBase::writeString(const TypeComponentUnit& target, const std::string& str, uint32_t& crc) {

    LOGC_T(getHost(), target, MSGDIR_SEND, "String write process is started => String : %s", str.c_str());

	if (!writeBlock(target, (uint8_t*)str.c_str(), str.size(), crc)) {
		LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write string to stream");
		return false;
	}

    LOGC_T(getHost(), target, MSGDIR_SEND, "String is written successfully => String : %s", str.c_str());

	return true;
}

bool MessageBase::writeNumber(const TypeComponentUnit& target, long number, uint32_t& crc) {

    uint8_t buffer[64] = {};
    *((long *) buffer) = htonll(number);

    LOGC_T(getHost(), target, MSGDIR_SEND, "Number write process is started => Number : %ld", number);

	if (!writeBlock(target, buffer, 64, crc)) {
		LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write char array to stream");
		return false;
	}

    LOGC_T(getHost(), target, MSGDIR_SEND, "Number is written successfully => Number : %ld", number);

	return true;
}

bool MessageBase::writeNumberList(const TypeComponentUnit& target, std::vector<long>& list, uint32_t& crc) {

    uint8_t buffer[64] = {};

    LOGC_T(getHost(), target, MSGDIR_SEND, "NumberList write process is started  => Count : %ld, First Number : %ld",
           list.size(), list[0]);

    for (auto number : list) {

        *((long *) buffer) = htonll(number);
        if (!writeBlock(target, buffer, 64, crc)) {
            LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write char array to stream");
            return false;
        }
    }

    LOGC_T(getHost(), target, MSGDIR_SEND, "NumberList is written successfully => Count : %ld, First Number : %ld",
            list.size(), list[0]);

    return true;
}

bool MessageBase::writeBinary(const TypeComponentUnit& target, const char* path, size_t size, uint32_t& crc) {

    LOGC_T(getHost(), target, MSGDIR_SEND, "File Binary write process is started at path : %s", path);

	int in = open(path, O_RDONLY);
	if (in == -1) {
		LOGC_E(getHost(), target, MSGDIR_SEND, "File Binary could not created or opened at path : %s", path);
		return false;
	}

    auto source = std::make_shared<ComponentUnit>(in);

	bool status = transferBinary(source, target, size, crc);

	close(in);

    LOGC_T(getHost(), target, MSGDIR_SEND, "File Binary is written successfully at path : %s", path);

	return status;

}

bool MessageBase::writeEndStream(const TypeComponentUnit& target, uint32_t& crc) {

	MessageBlockHeader block;

    LOGC_T(getHost(), target, MSGDIR_SEND, "End Stream write process is started");

    if (!writeBlockHeader(target, block, crc)) {

        LOGC_E(getHost(), target, MSGDIR_SEND, "End Stream can not be written");
        return false;
    }

    LOGC_T(getHost(), target, MSGDIR_SEND, "End Stream is written successfully");

	return true;
}

bool MessageBase::writeCRC(const TypeComponentUnit& target, uint32_t &crc) {

    uint8_t buffer[32] = {};
    uint32_t calcCRC = crc;
    *((uint32_t *) buffer) = htonl(crc);

    LOGC_T(getHost(), target, MSGDIR_SEND, "CRC write process is started => 0x%X", calcCRC);

    if (!writeBlock(target, buffer, 32, crc)) {
        LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write CRC to stream");
        return false;
    }

    LOGC_T(getHost(), target, MSGDIR_SEND, "CRC is written successfully => 0x%X", calcCRC);

    return true;
}

bool MessageBase::writeToStream(const TypeComponentUnit& target) {

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

const TypeHostUnit& MessageBase::getHost() {

    return host;
}
