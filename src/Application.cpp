//
// Created by Haluk AKGUNDUZ on 27/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include "Log.h"
#ifdef __WXWIDGETS__
#include "UserInterface.h"
#endif
#include "Console.h"

Console *c_ui;

int main(int argc, char** argv) {

	Log::setLogLevel(LEVEL_INFO);

	ConnInterface::initInterfaces();

	if (argc > 1) {

		c_ui = new Console();

		char cmd = argv[1][0];

		switch(cmd) {
			case 'l':
				LOG_S("Listing Interfaces .....");
				for (uint32_t i = 0; i < ConnInterface::getInterfaceCount(); i++) {
					ConnInterface *ci = ConnInterface::getInterface(i);
					LOG_S("%s : %s", sInterfaces[ci->type], ci->name.c_str());
				}
				break;
			case 'd':
				LOG_I("Running in Console Distributor Mode");
				break;
			case 'o':
				LOG_I("Running in Console Collector Mode");
				break;
			case 'c':
				LOG_I("Running in Console Client Mode");

				if (argc != 4) {
					LOG_E("Invalid Parameter, Must be \"Mode Interface Interface\"");
					break;
				}

				c_ui->clientRun((INTERFACES) atoi(argv[2]), (INTERFACES) atoi(argv[3]));
				break;
			default:
				break;
		}

		return 0;
	}

#ifdef __WXWIDGETS__
	return wxEntry(argc, argv);
#else
	return 0;
#endif

}
