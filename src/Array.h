//
// Created by Haluk AKGUNDUZ on 10.05.2018.
//

#ifndef BANKOR_ARRAY_H
#define BANKOR_ARRAY_H

#include "Util.h"

class Array {

public:

    uint8_t *data;
    size_t size;

    Array(const size_t size) : size(size) {

        data = new uint8_t[size];
        memset(data, 0, size);
    }

    Array(const Array &ref) : size(ref.size) {

        data = new uint8_t[size];
        memcpy(data, ref.data, size);
    }

    virtual ~Array() {

        delete[] data;
    }

    uint8_t * get() {

        return data;
    }

    size_t getSize() {

        return size;
    }

    void set(Array *ref) {

        if (ref != nullptr) {
            memcpy(data, ref->data, size);
        }
    }

    void set(uint8_t *ref) {

        if (ref != nullptr) {
            memcpy(data, ref, size);
        }
    }

    void reset() {

        memset(data, 0, size);
    }

    std::string getStr() {

        return Util::hex2str(data, size);
    }

    bool equal(const Array &ref) const {

        return memcmp(data, ref.data, size) == 0;
    }

    bool empty() const {

        for (int i = 0; i < size; i++)
        {
            if(data[i] > 0)
                return false;

        }
        return true;
    }

    bool compare(const Array &ref) const {

        return memcmp(data, ref.data, size) < 0;
    }
};

struct cmp_array {

    bool operator()(const Array &a, const Array &b) const {

        return a.compare(b);
    }
};

typedef std::vector<Array> TypeArrayList;


#endif //BANKOR_ARRAY_H
