//
// Created by Haluk AKGUNDUZ on 30.04.2018.
// Copyright (c) 2018 Haluk Akgunduz. All rights reserved.
//

#include "UserInterfaceController.h"

void UserInterfaceController::updateUICallback(void *context, int id, void *data) {

    ((UserInterfaceController*) context)->updateUIEvent(id, data);
}