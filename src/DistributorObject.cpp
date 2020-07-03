//
// Created by Haluk AKGUNDUZ on 03.07.2020.
//

#include "DistributorObject.h"

DistributorObject::DistributorObject() :
        DistributorObject(0, 0) {
}

DistributorObject::DistributorObject(DistributorObject &copy) :
        ComponentObject(copy) {
}

DistributorObject::DistributorObject(ComponentObject &copy) :
        ComponentObject(copy) {
}

DistributorObject::DistributorObject(const char* rootPath) :
        ComponentObject(COMP_DISTRIBUTOR, rootPath)  {
}

DistributorObject::DistributorObject(int id, long address) :
        ComponentObject(COMP_DISTRIBUTOR, id, address) {
}

DistributorObject::~DistributorObject() {

}
