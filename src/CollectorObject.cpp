//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#include "CollectorObject.h"

CollectorObject::CollectorObject() :
        ComponentObject(COMP_COLLECTOR) {

}

CollectorObject::CollectorObject(int id) :
        ComponentObject(COMP_COLLECTOR, NULL, id) {

}

CollectorObject::~CollectorObject() {

}

