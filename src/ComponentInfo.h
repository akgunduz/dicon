//
// Created by akgun on 7.07.2020.
//

#ifndef DICON_COMPONENTINFO_H
#define DICON_COMPONENTINFO_H

enum COMPONENT {

    COMP_DISTRIBUTOR,
    COMP_COLLECTOR,
    COMP_NODE,
    COMP_MAX
};

class ComponentInfo {

protected:

    long id{};

    COMPONENT type{COMP_DISTRIBUTOR};

    long address[COMP_MAX]{};

public:

    explicit ComponentInfo(COMPONENT = COMP_DISTRIBUTOR, long = 0, long = 0);
    ComponentInfo(const ComponentInfo&);

    long getID() const;
    void setID(long);

    COMPONENT getType() const;
    void setType(COMPONENT);

    long getAddress(COMPONENT) const;
    void setAddress(COMPONENT, long);

    long getAddress() const;
    void setAddress(long);

    static COMPONENT next(COMPONENT);
    static COMPONENT prev(COMPONENT);
};


#endif //DICON_COMPONENTINFO_H
