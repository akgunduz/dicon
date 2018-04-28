//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#ifndef BANKOR_COMPONENTOBJECT_H
#define BANKOR_COMPONENTOBJECT_H


class ComponentObject {

private:

    int id;

protected:

public:

    ComponentObject(int);

    virtual ~ComponentObject();

    int getID();

};


#endif //BANKOR_COMPONENTOBJECT_H
