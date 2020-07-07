//
// Created by akgun on 7.07.2020.
//

#ifndef DICON_COMPONENTINFO_H
#define DICON_COMPONENTINFO_H


class ComponentInfo {

    int id;

    long address;

public:

    ComponentInfo();
    ComponentInfo(int, long);

    int getID() const;
    void setID(int);

    long getAddress() const;
    void setAddress(long);

};


#endif //DICON_COMPONENTINFO_H
