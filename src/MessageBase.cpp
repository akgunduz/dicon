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

MessageBase::~MessageBase() {

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

bool MessageBase::onRead(const TypeComponentUnit& source, ssize_t nRead, const uv_buf_t *buf) {

    if (nRead == 0 || nRead == UV_EOF) {

        return false;
    }

//    LOGS_E(getHost(), "%ld : Data received, count : %d, bufPtr : %s", iter++,
//           nRead, Util::hex2str((uint8_t*)buf->base, nRead).c_str());

    uint32_t minContDataLength;
    size_t remaining = 0;

    auto *bufPtr = (uint8_t*)buf->base;

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

            readBlock(source, ptr, sizeof(MessageBlock));

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

    if (block.getSize() == 0 || block.getSign() != SIGNATURE) {

        LOGS_E(getHost(), "Block is invalid, type : %d, size : %d",
               block.getType(), block.getSize());

    }

    if (block.getSign() != SIGNATURE) {

        LOGS_E(getHost(), "Block Signature Mismatch, expected : 0x%X, read : 0x%X",
               SIGNATURE, block.getSign());

    } else {

        LOGS_T(getHost(), "Block is read successfully, Following block type : %d, size : %d",
               block.getType(), block.getSize());
    }

    return block.getSign() == SIGNATURE;
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

    LOGS_E(getHost(),
           "CRC is read successfully, => Read : 0x%X, Calculated : 0x%X", resCrc, crc);
    return resCrc == crc;
}


bool MessageBase::onWrite(const TypeComponentUnit& target, const uint8_t *buf, size_t size) {

    uv_buf_t bufPtr = uv_buf_init((char *) buf, size);

    crc = CRC::Calculate(buf, size, Util::crcTable, crc);

    auto *writeReq = (uv_write_t*) malloc(sizeof(uv_write_t));
    uv_write(writeReq, writeHandle, &bufPtr, 1, [](uv_write_t* req, int status) {

        if (status) {
            LOGP_E("Write request problem, error : %d!!!", status);
        }

        free(req);
    });

	return true;
}

bool MessageBase::writeBlock(const TypeComponentUnit& target, const MessageBlock& baseHeader) {

    LOGC_T(getHost(), target, MSGDIR_SEND, "BlockBase write process is started");

    baseHeader.fillBuffer(tmpBuf);

    if (!onWrite(target, tmpBuf, sizeof(MessageBlock))) {
        LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write BlockBase to stream");
        return false;
    }

    LOGC_T(getHost(), target, MSGDIR_SEND, "BlockBase is written successfully");

    return true;
}

bool MessageBase::writeHeader(const TypeComponentUnit& target) {

    LOGC_T(getHost(), target, MSGDIR_SEND, "Header write process is started");

    MessageBlock baseHeader(MSGHEADER_HEADER, sizeof(MessageHeader));

    if (!writeBlock(target, baseHeader)) {
        LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write Header BlockBase to stream");
        return false;
    }

    serializeHeader(tmpBuf);

	if (!onWrite(target, tmpBuf, header.getSize())) {
		LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write Header to stream");
		return false;
	}

    LOGC_T(getHost(), target, MSGDIR_SEND, "Header is written successfully");

	return true;
}

bool MessageBase::writeString(const TypeComponentUnit& target, const std::string& str) {

    LOGC_T(getHost(), target, MSGDIR_SEND, "String write process is started => String : %s", str.c_str());

    MessageBlock baseHeader(MSGHEADER_STRING, str.size());

    if (!writeBlock(target, baseHeader)) {
        LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write String BlockBase to stream");
        return false;
    }

	if (!onWrite(target, (uint8_t *) str.c_str(), str.size())) {
		LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write String to stream");
		return false;
	}

    LOGC_T(getHost(), target, MSGDIR_SEND, "String is written successfully => String : %s", str.c_str());

	return true;
}

bool MessageBase::writeNumber(const TypeComponentUnit& target, uint64_t number) {

    LOGC_T(getHost(), target, MSGDIR_SEND, "Number write process is started => Number : %ld", number);

    MessageBlock baseHeader(MSGHEADER_NUMBER, sizeof(uint64_t));

    if (!writeBlock(target, baseHeader)) {
        LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write Number BlockBase to stream");
        return false;
    }

    uint8_t buffer[8] = {};
    *((uint64_t *) buffer) = htonll(number);

	if (!onWrite(target, buffer, 8)) {
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

    if (!writeString(target, fileItem->getRefPath())) {
        LOGC_E(getHost(), target, MSGDIR_SEND, "File Binary can not write file path");
        return false;
    }

    MessageBlock baseHeader(MSGHEADER_BINARY, fileItem->getSize());

    if (!writeBlock(target, baseHeader)) {
        LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write Binary BlockBase to stream");
        return false;
    }

    size_t remaining = fileItem->getSize();

    FILE *file = std::fopen(fileItem->getPath().string().c_str(), "rb");

    if (file == nullptr) {
        LOGC_E(getHost(), target, MSGDIR_SEND, "File could not opened at path : %s",
               fileItem->getPath().string().c_str());
        return false;
    }

    do {

        int count = std::fread(tmpBuf, 1, std::min((size_t)TMP_BUFFER_SIZE, remaining), file);

        onWrite(target, tmpBuf, count);

        remaining -= count;

    } while(remaining);

    std::fclose(file);

    LOGC_T(getHost(), target, MSGDIR_SEND,
           "File Binary is written successfully at path : %s", fileItem->getPath().c_str());

	return true;

}

bool MessageBase::writeEndStream(const TypeComponentUnit& target) {

    LOGC_T(getHost(), target, MSGDIR_SEND, "EndStream write process is started => CRC : 0x%X", crc);

    MessageBlock baseHeader(MSGHEADER_END, sizeof(uint32_t));

    if (!writeBlock(target, baseHeader)) {
        LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write EndStream BlockBase to stream");
        return false;
    }

    uint8_t buffer[4] = {};
    *((uint32_t *) buffer) = htonl(crc);

    if (!onWrite(target, buffer, 4)) {
        LOGC_E(getHost(), target, MSGDIR_SEND, "Can not write EndStream to stream");
        return false;
    }

    LOGC_T(getHost(), target, MSGDIR_SEND, "End Stream is written successfully => CRC : 0x%X", crc);

	return true;
}

bool MessageBase::writeToStream(const TypeComponentUnit& target) {

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
