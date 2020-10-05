//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include <cstring>
#include <cassert>

#include "MessageBase.h"
#include "Log.h"
#include "Util.h"
#include "Platform.h"

MessageBase::MessageBase(const TypeHostUnit& _host)
		: host(_host) {

    readParser[MSGHEADER_HEADER] = &MessageBase::readHeader;
    readParser[MSGHEADER_STRING] = &MessageBase::readString;
    readParser[MSGHEADER_NUMBER] = &MessageBase::readNumber;
    readParser[MSGHEADER_BINARY] = &MessageBase::readBinary;
    readParser[MSGHEADER_END] = &MessageBase::readEndStream;
}

MessageBase::MessageBase(const TypeHostUnit& host, COMPONENT targetType, MSG_TYPE msgType, STREAM_TYPE streamType)
		: MessageBase(host) {

    header.setType(msgType);
    header.setStream(streamType);
    header.setOwner(*getHost()->forkCommUnit(targetType));
}

MessageBase::~MessageBase() = default;

bool MessageBase::onRead(const TypeComponentUnit& source, const uint8_t *buffer, size_t nRead) {

//    LOGS_E(getHost(), "%ld : Data received, count : %3d, bufPtr : %s", iter++,
//           nRead, Util::hex2str(buffer, nRead).c_str());

    size_t minContDataLength;
    size_t remaining = 0;

    auto *bufPtr = buffer;

    do {

        if (nRead <= 0) {
            break;
        }

        minContDataLength = state == MSGSTATE_INIT ? sizeof(MessageBlock) : block.getSize();

        remaining = minContDataLength - (state != MSGSTATE_BINARY ? tmpBufPos : binBufPos);

        if (nRead < remaining && state != MSGSTATE_BINARY) {

            memcpy(tmpBuf + tmpBufPos, bufPtr, nRead);
            tmpBufPos += nRead;

//            LOGP_E("Partial Read, nRead : %d, remaining : %d, tmpBuf : %s",
//                   nRead, remaining, Util::hex2str(tmpBuf, tmpBufPos).c_str());

            return false;
        }

        if (tmpBufPos) {

            memcpy(tmpBuf + tmpBufPos, bufPtr, remaining);
//            LOGP_E("Copied remaining, : %d, tmpBuf : %s",
//                   remaining, Util::hex2str(tmpBuf, tmpBufPos + remaining).c_str());

            bufPtr += remaining;
        }

        auto *ptr = tmpBufPos ? tmpBuf : bufPtr;

        if (state == MSGSTATE_INIT) {

            crc = CRC::Calculate(ptr, sizeof(MessageBlock), Util::crcTable, crc);

            if (!readBlock(source, ptr, sizeof(MessageBlock))) {

                tmpBufPos = 0;

                crc = 0;

                return false;
            }

            if (block.getType() == MSGHEADER_BINARY) {

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

            if (block.getType() != MSGHEADER_END) {

                crc = CRC::Calculate(ptr, block.getSize(), Util::crcTable, crc);
            }

            (this->*readParser[static_cast<MSG_HEADER>(block.getType())])(source, ptr, block.getSize());

            if (block.getType() == MSGHEADER_END) {

                return true;

            }

            state = MSGSTATE_INIT;

            if (!tmpBufPos) {

                bufPtr += block.getSize();
            }

        } else if (state == MSGSTATE_BINARY) {

            size_t readSize = nRead > remaining ? remaining : nRead;

            crc = CRC::Calculate(ptr, readSize, Util::crcTable, crc);

            readBinary(source, ptr, readSize);

            binBufPos += readSize;

            if (nRead >= remaining) {

                state = MSGSTATE_INIT;

                bufPtr += readSize;

                strings.pop_back();

            } else {

                break;
            }

        }

        tmpBufPos = 0;

        nRead -= remaining;

    } while(true);

    return false;
}

bool MessageBase::readBlock(const TypeComponentUnit& source, const uint8_t* buffer, size_t size) {

    LOGS_T(getHost(), "Block read process is started");

    block.parseBuffer(buffer);

    if (block.getSize() == 0 || block.getType() == MSGHEADER_MAX) {

        LOGS_E(getHost(), "Block is invalid, type : %d, size : %d, data : %s",
               block.getType(), block.getSize(), Util::hex2str(buffer, size).c_str());

        return false;

    }

    if (block.getSign() != SIGNATURE) {

        LOGS_E(getHost(), "Block Signature Mismatch, expected : 0x%X, read : 0x%X, data : %s",
               SIGNATURE, block.getSign(), Util::hex2str(buffer, size).c_str());

        return false;

    }

    LOGS_T(getHost(), "Block is read successfully, Following block type : %d, size : %d",
               block.getType(), block.getSize());

    return true;
}

bool MessageBase::readHeader(const TypeComponentUnit& source, const uint8_t* buffer, size_t size) {

    LOGS_T(getHost(), "Header read process is started");

    deSerializeHeader(buffer);

    grabOwner(source);

    LOGS_T(getHost(), "Header is read successfully");

	return true;
}

bool MessageBase::readString(const TypeComponentUnit& source, const uint8_t* buffer, size_t size) {

    LOGC_T(getHost(), source, MSGDIR_RECEIVE, "String read process is started => String Length: %d", size);

    strings.emplace_back((char *)buffer, size);

    LOGC_T(getHost(), source, MSGDIR_RECEIVE, "String is read successfully => String : %s", strings.back().c_str());

	return true;
}

bool MessageBase::readNumber(const TypeComponentUnit& source, const uint8_t* buffer, size_t size) {

    LOGC_T(getHost(), source, MSGDIR_RECEIVE, "Number read process is started");

    numbers.emplace_back(ntohll(*((uint64_t*) buffer)));

    LOGC_T(getHost(), source, MSGDIR_RECEIVE, "Number is read successfully, number => %ld", numbers.back());

	return true;
}

bool MessageBase::readBinary(const TypeComponentUnit& source, const uint8_t* buffer, size_t size) {

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

bool MessageBase::readEndStream(const TypeComponentUnit& source, const uint8_t* buffer, size_t size) {

    LOGC_T(getHost(), source, MSGDIR_RECEIVE, "CRC read process is started");

    uint32_t resCrc = ntohl(*((uint32_t *) buffer));

    LOGC_T(getHost(), source, MSGDIR_RECEIVE,
           "CRC is read successfully, => Read : 0x%X, Calculated : 0x%X", resCrc, crc);

    return resCrc == crc;
}

bool MessageBase::onWrite(const TypeComponentUnit& target, MSG_HEADER blockType, const uint8_t *buffer, size_t size) {

    if (blockType == MSGHEADER_MAX) {

        crc = CRC::Calculate(buffer, size, Util::crcTable, crc);

        writeData(target, buffer, size);

//        LOGS_E(getHost(), "%ld : Data     sent, count : %3d, bufPtr : %s", iter++,
//               size, Util::hex2str(buffer, size).c_str());

        return true;

    }

    size_t newPos = tmpBufPos + sizeof(MessageBlock) + size;

    if (newPos > TMP_BUFFER_SIZE) {

        writeData(target, tmpBuf, tmpBufPos);

//        LOGS_E(getHost(), "%ld : Data     sent, count : %3d, bufPtr : %s", iter++,
//               tmpBufPos, Util::hex2str(tmpBuf, tmpBufPos).c_str());

        tmpBufPos = 0;
    }

    block.set(blockType, (uint32_t)size);

    block.fillBuffer(tmpBuf + tmpBufPos);

    crc = CRC::Calculate(tmpBuf + tmpBufPos, sizeof(MessageBlock), Util::crcTable, crc);

    tmpBufPos += sizeof(MessageBlock);

    if (blockType == MSGHEADER_END) {

        *((uint32_t *) buffer) = htonl(crc);
    }

    if (blockType != MSGHEADER_BINARY) {

        memcpy(tmpBuf + tmpBufPos, buffer, size);

        crc = CRC::Calculate(buffer, size, Util::crcTable, crc);

        tmpBufPos += size;
    }

    if (blockType == MSGHEADER_BINARY || blockType == MSGHEADER_END) {

        writeData(target, tmpBuf, tmpBufPos);

//        LOGS_E(getHost(), "%ld : Data     sent, count : %3d, bufPtr : %s", iter++,
//               tmpBufPos, Util::hex2str(tmpBuf, tmpBufPos).c_str());

        tmpBufPos = 0;
    }

    return true;
}

bool MessageBase::writeHeader(const TypeComponentUnit& target) {

    LOGC_T(getHost(), target, MSGDIR_SEND, "Header write process is started");

    uint8_t buffer[sizeof(MessageHeader)] = {};

    serializeHeader(buffer);

	if (!onWrite(target, MSGHEADER_HEADER, buffer, sizeof(MessageHeader))) {
		LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write Header to stream");
		return false;
	}

    LOGC_T(getHost(), target, MSGDIR_SEND, "Header is written successfully");

	return true;
}

bool MessageBase::writeString(const TypeComponentUnit& target, const std::string& str) {

    LOGC_T(getHost(), target, MSGDIR_SEND, "String write process is started => String : %s", str.c_str());

	if (!onWrite(target, MSGHEADER_STRING, (uint8_t *) str.c_str(), str.size())) {
		LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write String to stream");
		return false;
	}

    LOGC_T(getHost(), target, MSGDIR_SEND, "String is written successfully => String : %s", str.c_str());

	return true;
}

bool MessageBase::writeNumber(const TypeComponentUnit& target, uint64_t number) {

    LOGC_T(getHost(), target, MSGDIR_SEND, "Number write process is started => Number : %ld", number);

    uint8_t buffer[sizeof(uint64_t)] = {};

    *((uint64_t *) buffer) = htonll(number);

	if (!onWrite(target, MSGHEADER_NUMBER, buffer, sizeof(uint64_t))) {
		LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write Number to stream");
		return false;
	}

    LOGC_T(getHost(), target, MSGDIR_SEND, "Number is written successfully => Number : %ld", number);

	return true;
}

bool MessageBase::writeBinary(const TypeComponentUnit& target,
                              const TypeFileItem& fileItem) {

    LOGC_T(getHost(), target, MSGDIR_SEND,
           "File Binary write process is started at path : %s", fileItem->getPath().c_str());

    if (!writeString(target, fileItem->getRefPath().string())) {
        LOGC_E(getHost(), target, MSGDIR_SEND, "File Binary can not write file path");
        return false;
    }

    if (!onWrite(target, MSGHEADER_BINARY, nullptr, (size_t) fileItem->getSize())) {
        LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write Number to stream");
        return false;
    }

    FILE *file = std::fopen(fileItem->getPath().string().c_str(), "rb");

    if (file == nullptr) {
        LOGC_E(getHost(), target, MSGDIR_SEND, "File could not opened at path : %s",
               fileItem->getPath().string().c_str());
        return false;
    }

    auto remaining = (size_t) fileItem->getSize();

    do {

        size_t count = std::fread(tmpBuf, 1, std::min((size_t)TMP_BUFFER_SIZE, remaining), file);

        onWrite(target, MSGHEADER_MAX, tmpBuf, count);

        remaining -= count;

    } while(remaining);

    std::fclose(file);

    LOGC_T(getHost(), target, MSGDIR_SEND,
           "File Binary is written successfully at path : %s", fileItem->getPath().c_str());

	return true;

}

bool MessageBase::writeEndStream(const TypeComponentUnit& target) {

    uint8_t buffer[sizeof(uint32_t)] = {};

    if (!onWrite(target, MSGHEADER_END, buffer, sizeof(uint32_t))) {
        LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write EndStream to stream");
        return false;
    }

    LOGC_T(getHost(), target, MSGDIR_SEND, "End Stream is written successfully");

	return true;
}

bool MessageBase::writeToStream(const TypeComponentUnit& target, TypeWriteCB _writeData) {

    assert(_writeData != nullptr);

    writeData = _writeData;

	if (!writeHeader(target)) {
		return false;
	}

    if (!writeMessageStream(target)) {
        return false;
    }

	return writeEndStream(target);
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
