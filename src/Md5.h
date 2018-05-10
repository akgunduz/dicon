//
// Created by Haluk Akgunduz on 10/30/15.
//

#ifndef BANKOR_MD5_H
#define BANKOR_MD5_H

#include "Common.h"
#include "Util.h"
#include "ContentItem.h"
#include "Array.h"

class Md5 : public Array {

public:


    Md5() : Array(MD5_DIGEST_LENGTH) {
    }

    Md5(const char *path) : Md5() {

        load(path);
    }

    void save(Md5 *ref, const char *path = nullptr) {

        set(ref);

        if (path != nullptr) {
            FILE *md5file = fopen(path, "w");
            fwrite(Util::hex2str(data, MD5_DIGEST_LENGTH).c_str(), 1, MD5_DIGEST_LENGTH * 2, md5file);
            fclose(md5file);
        }
    }

    bool load(const char *path) {

        FILE *md5file = fopen(path, "r");
        if (md5file == nullptr) {
            return false;
        }

        char buf[BUFFER_SIZE];
        fgets(buf, MD5_DIGEST_LENGTH * 2 + 1, md5file);
        fclose(md5file);

        return Util::str2hex(data, buf, MD5_DIGEST_LENGTH);
    }

};

typedef std::vector<Md5> TypeMD5List;

#endif //BANKOR_MD5_H
