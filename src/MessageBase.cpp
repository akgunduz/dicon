//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "MessageBase.h"
#include "Log.h"
#include "Util.h"
#include "CommTCP.h"
#include "UnixSocket.h"

MessageBase::MessageBase(const TypeHostUnit& host)
		: host(host) {

    readParser[MSGHEADER_HEADER] = &MessageBase::readHeader;
    readParser[MSGHEADER_STRING] = &MessageBase::readString;
    readParser[MSGHEADER_NUMBER] = &MessageBase::readNumber;
    readParser[MSGHEADER_BINARY] = &MessageBase::readBinary;
    readParser[MSGHEADER_END] = &MessageBase::readEndStream;
}

MessageBase::MessageBase(const TypeHostUnit& host, const TypeComponentUnit& target,
                         MSG_TYPE msgType, STREAM_TYPE streamType)
		: MessageBase(host) {

    header.setType(msgType);
    header.setStream(streamType);
    header.setOwner(host->getUnit(target->getType()));
}

TypeReadCB MessageBase::getReadCB(const TypeComponentUnit& source) {

    if (source->getAddress().getInterface() == COMMINTERFACE_TCPIP) {

        return CommTCP::getReadCB(source);
    }

    return UnixSocket::getReadCB(source);
}

TypeWriteCB MessageBase::getWriteCB(const TypeComponentUnit&  target) {

    if (target->getAddress().getInterface() == COMMINTERFACE_TCPIP) {

        return CommTCP::getWriteCB(target);
    }

    return UnixSocket::getWriteCB(target);
}

bool MessageBase::transferBinary(const TypeComponentUnit& source, const TypeComponentUnit& target, size_t size, uint32_t& crc) {

    uint8_t buf[BUFFER_SIZE] = {};

    bool error = false;
    size_t readSize = 0;
    uint32_t fakeCrc = 0;

    do {

        if (size > BUFFER_SIZE) {
            readSize = BUFFER_SIZE;
            size -= readSize;
        } else {
            readSize = size;
            size = 0;
        }

        if (!readBlockDeprecated(source, buf, readSize, crc)) {
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

bool MessageBase::readBlockDeprecated(const TypeComponentUnit& source, uint8_t *buf, size_t size, uint32_t& crc) {

    size_t offset = 0;
	bool busy = false;

	do {

        size_t count = getReadCB(source)(source, buf + offset, (size_t) size);

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

bool MessageBase::onRead(const TypeComponentUnit& source, ssize_t nRead, const uv_buf_t *buf) {

    if (nRead == 0 || nRead == UV_EOF) {

        return false;
    }

    LOGS_E(getHost(), "%ld : Data received, count : %d, bufPtr : %s", iter++,
           nRead, Util::hex2str((uint8_t*)buf->base, nRead).c_str());

    uint32_t minContDataLength;
    size_t remaining = 0;

    auto *bufPtr = (uint8_t*)buf->base;

    do {

        if (nRead <= 0) {
            break;
        }

        minContDataLength = state == MSGSTATE_INIT ? sizeof(MessageBlock) : block.size;

        remaining = minContDataLength - (state != MSGSTATE_BINARY ? tmpBufPos : binBufPos);

        if (nRead < remaining && state != MSGSTATE_BINARY) {

            memcpy(tmpBuf + tmpBufPos, bufPtr, nRead);
            tmpBufPos += nRead;

            LOGP_E("Partial Read, nRead : %d, remaining : %d, tmpBuf : %s",
                   nRead, remaining, Util::hex2str(tmpBuf, tmpBufPos).c_str());

            return false;
        }

        if (tmpBufPos) {

            memcpy(tmpBuf + tmpBufPos, bufPtr, remaining);
            LOGP_E("Copied remaining, : %d, tmpBuf : %s",
                   remaining, Util::hex2str(tmpBuf, tmpBufPos + remaining).c_str());

            bufPtr += remaining;
        }

        auto *ptr = tmpBufPos ? tmpBuf : bufPtr;

        if (state == MSGSTATE_INIT) {

            readBlock(source, ptr, sizeof(MessageBlock), crc);

            if (block.type == MSGHEADER_BINARY) {
                LOGP_E("Binary touch 1");
                binBufPos = 0;

                state = MSGSTATE_BINARY;

                assert(!strings.empty());

                std::filesystem::remove(getHost()->getRootPath() / strings.back());

            } else {

                state = MSGSTATE_DATA;
            }

            if (!tmpBufPos) {

                bufPtr += sizeof(MessageBlock);
            }

        } else if (state == MSGSTATE_DATA) {

            (this->*readParser[static_cast<MSG_HEADER>(block.type)])(source, ptr, block.size, crc);

            if (block.type == MSGHEADER_END) {

                return true;
            }

            state = MSGSTATE_INIT;

            if (!tmpBufPos) {

                bufPtr += block.size;
            }

        } else if (state == MSGSTATE_BINARY) {
            LOGP_E("Binary touch 2");
            size_t readSize = nRead > remaining ? remaining : nRead;

            readBinary(source, ptr, readSize, crc);

            binBufPos += readSize;

            if (nRead >= remaining) {

                state = MSGSTATE_INIT;

                bufPtr += readSize;

                strings.pop_back();
            }

        }

        tmpBufPos = 0;

        nRead -= remaining;

    } while(true);

    return false;
}

bool MessageBase::readBlock(const TypeComponentUnit& source, const uint8_t* buffer,size_t size, uint32_t& crc) {
  //  LOGS_E(getHost(), "%d. Block : %s", iter++, Util::hex2str(buffer, sizeof(MessageBlock)).c_str());
    LOGS_T(getHost(), "Block read process is started");

    auto* ptr = buffer;

    block.sign = ntohs(*((uint16_t *) ptr)); ptr += 2;
    block.type = ntohs(*((uint16_t *) ptr)); ptr += 2;
    block.size = ntohl(*((uint32_t *) ptr)); ptr += 4;

    if (block.size == 0 || block.sign != SIGNATURE) {

        LOGS_E(getHost(), "Block is invalid, len : %d, type : %d", block.size, block.type);
        assert(false);
    }


    if (block.sign != SIGNATURE) {

        LOGS_E(getHost(), "Block Signature Mismatch, expected : 0x%X, read : 0x%X",
               SIGNATURE, block.sign);

    } else {

        LOGS_T(getHost(), "Block is read successfully, Following block type : %d, size : %d", block.type, block.size);
    }

    return block.sign == SIGNATURE;
}

bool MessageBase::readHeader(const TypeComponentUnit& source, const uint8_t* buffer, size_t size, uint32_t& crc) {
  //  LOGS_E(getHost(), "%d. Header : %s", iter++, Util::hex2str(buffer, size).c_str());
    LOGS_T(getHost(), "Header read process is started");

    deSerializeHeader(buffer);

    grabOwner(source);

    LOGS_T(getHost(), "Header is read successfully");

	return true;
}

bool MessageBase::readString(const TypeComponentUnit& source, const uint8_t* buffer, size_t size, uint32_t& crc) {

    LOGC_T(getHost(), source, MSGDIR_RECEIVE, "String read process is started => String Length: %d", size);

    strings.emplace_back((char *)buffer, size);

    LOGC_T(getHost(), source, MSGDIR_RECEIVE, "String is read successfully => String : %s", strings.back().c_str());

	return true;
}

bool MessageBase::readNumber(const TypeComponentUnit& source, const uint8_t* buffer, size_t size, uint32_t& crc) {

    LOGC_T(getHost(), source, MSGDIR_RECEIVE, "Number read process is started");

    numbers.emplace_back(ntohll(*((uint64_t*) buffer)));

    LOGC_T(getHost(), source, MSGDIR_RECEIVE, "Number is read successfully, number => %ld", numbers.back());

	return true;
}

bool MessageBase::readBinary(const TypeComponentUnit& source, const uint8_t* buffer, size_t size, uint32_t& crc) {

    std::filesystem::path filePath = getHost()->getRootPath() / strings.back();

    LOGC_T(getHost(), source, MSGDIR_RECEIVE, "Read File is started at path : %s", filePath.c_str());

    std::filesystem::create_directories(filePath.parent_path());

    FILE *file = std::fopen(filePath.string().c_str(), "ab");

	if (file == nullptr) {
        LOGC_E(getHost(), source, MSGDIR_RECEIVE, "Read File could not created or opened at path : %s", filePath.c_str());
		return false;
	}

    std::fwrite(buffer, 1, size, file);

    std::fclose(file);

    LOGC_T(getHost(), source, MSGDIR_RECEIVE, "File is read successfully => at path : %s", filePath.c_str());

	return true;

}

bool MessageBase::readEndStream(const TypeComponentUnit& source, const uint8_t* buffer, size_t size, uint32_t &crc) {

    uint32_t resCrc;
    uint32_t calcCrc = crc;

    LOGC_T(getHost(), source, MSGDIR_RECEIVE, "CRC read process is started, => Calculated : 0x%X", calcCrc);

    resCrc = ntohl(*((uint32_t *) tmpBuf));

    LOGC_T(getHost(), source, MSGDIR_RECEIVE, "CRC is read successfully, => Read : 0x%X, Calculated : 0x%X", resCrc, calcCrc);

    return resCrc == calcCrc;
}


bool MessageBase::writeBlock(const TypeComponentUnit& target, const uint8_t *buf, size_t size, uint32_t& crc) {

	size_t offset = 0;
	bool busy = false;

	do {

	    size_t count = getWriteCB(target)(target, buf + offset, size);

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

bool MessageBase::writeBlockHeader(const TypeComponentUnit& target, MessageBlockHeader &blockHeader, uint32_t& crc) {

    uint8_t *p = tmpBuf;
    *((uint16_t *) p) = htons(blockHeader.getType()); p += 2;
    *((uint16_t *) p) = htons(blockHeader.getCount()); p += 2;
    for (uint16_t i = 0; i < 3; i++) {
        *((uint32_t *) p) = htonl(blockHeader.get(i)); p += 4;
    }

    LOGC_T(getHost(), target, MSGDIR_SEND, "Block header write process is started => Type : %s, Count : %d",
           MessageType::getBlockName(blockHeader.getType()), blockHeader.getCount());

    if (!writeBlock(target, tmpBuf, p - tmpBuf, crc)) {
        LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write block header to stream");
        return false;
    }

    LOGC_T(getHost(), target, MSGDIR_SEND, "Block header is written successfully => Type : %s, Count : %d",
           MessageType::getBlockName(blockHeader.getType()), blockHeader.getCount());

    for (uint16_t i = 0; i < blockHeader.getCount(); i++) {
        LOGC_T(getHost(), target, MSGDIR_SEND, "Block Header Data[%d] : %d", i, blockHeader.get(i));
    }

    LOGP_E("Write => Len : %d, Block : %s", p - tmpBuf, Util::hex2str(tmpBuf, p - tmpBuf).c_str());

    return true;
}

bool MessageBase::writeBlockBase(const TypeComponentUnit& target, const MessageBlock& baseHeader, uint32_t& crc) {

    LOGC_T(getHost(), target, MSGDIR_SEND, "BlockBase write process is started");

    uint8_t *p = tmpBuf;
    *((uint16_t *) p) = htons(baseHeader.sign); p += 2;
    *((uint16_t *) p) = htons(baseHeader.type); p += 2;
    *((uint32_t *) p) = htonl(baseHeader.size); p += 4;

    if (!writeBlock(target, tmpBuf, sizeof(MessageBlock), crc)) {
        LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write BlockBase to stream");
        return false;
    }

    LOGC_T(getHost(), target, MSGDIR_SEND, "BlockBase is written successfully");

//    LOGP_E("Write BlockBase => Len : %d, Data   : %s", sizeof(MessageBlock),
//           Util::hex2str(tmpBuf, sizeof(MessageBlock)).c_str());

    return true;
}

bool MessageBase::writeHeader(const TypeComponentUnit& target, uint32_t& crc) {

    LOGC_T(getHost(), target, MSGDIR_SEND, "Header write process is started");

    MessageBlock baseHeader;
    baseHeader.type = MSGHEADER_HEADER;
    baseHeader.size = sizeof(MessageHeader);

    if (!writeBlockBase(target, baseHeader, crc)) {
        LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write Header BlockBase to stream");
        return false;
    }

    serializeHeader(tmpBuf);

	if (!writeBlock(target, tmpBuf, header.getSize(), crc)) {
		LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write Header to stream");
		return false;
	}

    LOGC_T(getHost(), target, MSGDIR_SEND, "Header is written successfully");

//    LOGP_E("Write Header => Len : %d, Data   : %s", header.getSize(),
//    Util::hex2str(tmpBuf, header.getSize()).c_str());

	return true;
}

bool MessageBase::writeString(const TypeComponentUnit& target, const std::string& str, uint32_t& crc) {

    LOGC_T(getHost(), target, MSGDIR_SEND, "String write process is started => String : %s", str.c_str());

    MessageBlock baseHeader;
    baseHeader.type = MSGHEADER_STRING;
    baseHeader.size = str.size();

    if (!writeBlockBase(target, baseHeader, crc)) {
        LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write String BlockBase to stream");
        return false;
    }

	if (!writeBlock(target, (uint8_t*)str.c_str(), str.size(), crc)) {
		LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write String to stream");
		return false;
	}

    LOGC_T(getHost(), target, MSGDIR_SEND, "String is written successfully => String : %s", str.c_str());

//    LOGP_E("Write String => Len : %d, Data   : %s", str.size(),
//           Util::hex2str((const uint8_t*)str.c_str(), str.size()).c_str());

	return true;
}

bool MessageBase::writeNumber(const TypeComponentUnit& target, uint64_t number, uint32_t& crc) {

    LOGC_T(getHost(), target, MSGDIR_SEND, "Number write process is started => Number : %ld", number);

    MessageBlock baseHeader;
    baseHeader.type = MSGHEADER_NUMBER;
    baseHeader.size = sizeof(uint64_t);

    if (!writeBlockBase(target, baseHeader, crc)) {
        LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write Number BlockBase to stream");
        return false;
    }

    uint8_t buffer[8] = {};
    *((uint64_t *) buffer) = htonll(number);

	if (!writeBlock(target, buffer, 8, crc)) {
		LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write Number to stream");
		return false;
	}

    LOGC_T(getHost(), target, MSGDIR_SEND, "Number is written successfully => Number : %ld", number);

	return true;
}

bool MessageBase::writeBinary(const TypeComponentUnit& target,
                              const TypeFileItem& fileItem, uint32_t& crc) {

    LOGC_T(getHost(), target, MSGDIR_SEND,
           "File Binary write process is started at path : %s", fileItem->getPath().c_str());

    if (!writeString(target, fileItem->getRefPath(), crc)) {
        LOGS_E(getHost(), "File Binary can not write file path");
        return false;
    }

    MessageBlock baseHeader;
    baseHeader.type = MSGHEADER_BINARY;
    baseHeader.size = fileItem->getSize();

    if (!writeBlockBase(target, baseHeader, crc)) {
        LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write Binary BlockBase to stream");
        return false;
    }

	int in = open(fileItem->getPath().string().c_str(), O_RDONLY);
	if (in == -1) {
		LOGC_E(getHost(), target, MSGDIR_SEND,
         "File Binary could not created or opened at path : %s", fileItem->getPath().c_str());
		return false;
	}

    auto source = std::make_shared<ComponentUnit>(in, 0);

	bool status = transferBinary(source, target, fileItem->getSize(), crc);

	close(in);

    LOGC_T(getHost(), target, MSGDIR_SEND,
           "File Binary is written successfully at path : %s", fileItem->getPath().c_str());

	return status;

}

bool MessageBase::writeEndStream(const TypeComponentUnit& target, uint32_t& crc) {

    LOGC_T(getHost(), target, MSGDIR_SEND, "EndStream write process is started => CRC : 0x%X", crc);

    MessageBlock baseHeader;
    baseHeader.type = MSGHEADER_END;
    baseHeader.size = sizeof(uint32_t);

    if (!writeBlockBase(target, baseHeader, crc)) {
        LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write EndStream BlockBase to stream");
        return false;
    }

    uint8_t buffer[4] = {};
    uint32_t calcCRC = crc;
    *((uint32_t *) buffer) = htonl(crc);

    if (!writeBlock(target, buffer, 4, crc)) {
        LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write EndStream to stream");
        return false;
    }

    LOGC_T(getHost(), target, MSGDIR_SEND, "End Stream is written successfully => 0x%X", crc);

//    LOGP_E("Write End => Len : %d, CRC    : %s", 4,
//           Util::hex2str(buffer, 4).c_str());

	return true;
}

bool MessageBase::writeToStream(const TypeComponentUnit& target) {

    uint32_t crc = 0;

	if (!writeHeader(target, crc)) {
		return false;
	}

    if (!writeMessageStream(target, crc)) {
        return false;
    }

	return writeEndStream(target, crc);
}

const TypeHostUnit& MessageBase::getHost() {

    return host;
}

bool MessageBase::deSerializeHeader(const uint8_t *buffer) {

    return header.deSerialize(buffer);
}

void MessageBase::grabOwner(const TypeCommUnit& unit) {

    header.grabOwner(unit);
}

void MessageBase::serializeHeader(uint8_t *buffer) {

    header.serialize(buffer);
}

MessageHeader& MessageBase::getHeader() {

    return header;
}
