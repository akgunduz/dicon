//
// Created by Haluk Akgunduz on 10/30/15.
//

#ifndef BANKOR_MD5_H
#define BANKOR_MD5_H

#include "Common.h"
#include "Util.h"
#include "ContentItem.h"

class Md5 {

public:
    uint8_t data[MD5_DIGEST_LENGTH];

    Md5() {
        memset(data, 0, MD5_DIGEST_LENGTH);
    }

    void set(Md5 *ref, const char *path = nullptr) {

        memcpy(data, ref->data, MD5_DIGEST_LENGTH);

        if (path != nullptr) {
            FILE *md5file = fopen(path, "w");
            fwrite(Util::hex2str(data, MD5_DIGEST_LENGTH).c_str(), 1, MD5_DIGEST_LENGTH * 2, md5file);
            fclose(md5file);
        }
    }

    bool get(const char *path) {

        FILE *md5file = fopen(path, "r");
        if (md5file == nullptr) {
            return false;
        }

        char buf[BUFFER_SIZE];
        fgets(buf, MD5_DIGEST_LENGTH * 2 + 1, md5file);
        fclose(md5file);

        return Util::str2hex(data, buf, MD5_DIGEST_LENGTH);
    }

    void reset() {
        memset(data, 0, MD5_DIGEST_LENGTH);
    }

    std::string getStr() {
        return Util::hex2str(data, MD5_DIGEST_LENGTH);
    }

    bool equal(Md5 *ref) {
        return memcmp(data, ref->data, MD5_DIGEST_LENGTH) == 0;
    }
};


#endif //BANKOR_MD5_H
