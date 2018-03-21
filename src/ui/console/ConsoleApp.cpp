//
// Created by Haluk AKGUNDUZ on 27/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Log.h"
#include "ConsoleApp.h"

int main(int argc, char** argv) {

	if (argc > 1) {

        Console *c_ui = new Console();

		char cmd = argv[1][0];

		Connector::createDevices();

		switch(cmd) {
			case 'l':
				LOG_S("Listing Interfaces .....");
				for (uint32_t i = 0; i < Connector::getCount(); i++) {
					LOG_S("%s : %s", sInterfaces[Connector::getDevice(i)->getType()], Connector::getDevice(i)->getName());
				}
				break;
			case 'd':
				LOG_I("Running in Console Distributor Mode");
				break;
			case 'o':
				LOG_I("Running in Console Collector Mode");
				break;
			case 'n':
				LOG_I("Running in Console Node Mode");

				if (argc != 4) {
					LOG_E("Invalid Parameter, Must be \"Mode Interface Interface\"");
					break;
				}

                c_ui->nodeRun(atoi(argv[2]), atoi(argv[3]));
				break;
			default:
				break;
		}

		return 0;
	}

	return 0;
}

