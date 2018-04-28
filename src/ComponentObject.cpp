//
// Created by Haluk AKGUNDUZ on 28.04.2018.
//

#include "ComponentObject.h"

ComponentObject::ComponentObject(int id) : id(id) {


}

ComponentObject::~ComponentObject() {

}

int ComponentObject::getID() {

    return this->id;
}
