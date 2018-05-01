//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#include "CollectorObject.h"

CollectorObject::CollectorObject() :
        ComponentObject(COMP_COLLECTOR, 0) {

}

CollectorObject::CollectorObject(int id) :
        ComponentObject(COMP_COLLECTOR, id) {

}

CollectorObject::~CollectorObject() {

}

