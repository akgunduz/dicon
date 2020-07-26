//
// Created by Haluk AKGUNDUZ on 23.07.2020.
//

#include "Test.h"

int listDevices() {

    DeviceList *deviceList = DeviceList::getInstance();

    PRINT("Listing Interfaces .....");

    for (int j = 0; j < deviceList->getCount(); j++) {
        PRINT("%s : %s", InterfaceTypes::getName(deviceList->get(j)->getType()),
              deviceList->get(j)->getName());
    }

    return 0;
}

bool parseParameters(int argc, char** argv, int *interfaceID,
                     LOGLEVEL* logLevel, int* componentCount) {

    for (int i = 1; i < argc; i++) {

        if (!strcmp(argv[i], "-l")) {

            return listDevices();

        } else if (!strcmp(argv[i], "-i")) {

            if (argc > i + 1) {

                if (isdigit(argv[++i][0])) {
                    interfaceID[0] = atoi(argv[i]);

                } else {
                    return false;
                }

                if (isdigit(argv[++i][0])) {
                    interfaceID[1] = atoi(argv[i]);

                } else {
                    return false;
                }
            }

        } else if (!strcmp(argv[i], "-d")) {

            componentCount[COMP_DISTRIBUTOR] = 1;

        } else if (!strcmp(argv[i], "-c")) {

            if (isdigit(argv[++i][0])) {
                componentCount[COMP_COLLECTOR] =  atoi(argv[i]);

            } else {

                return false;
            }

        } else if (!strcmp(argv[i], "-n")) {

            if (isdigit(argv[++i][0])) {
                componentCount[COMP_NODE] =  atoi(argv[i]);

            } else {

                return false;
            }

        } else if (!strcmp(argv[i], "-g")) {

            if (argc > i + 1) {

                if (isdigit(argv[++i][0])) {
                    logLevel[0] = (LOGLEVEL)atoi(argv[i]);

                } else {
                    return false;
                }

                if (isdigit(argv[++i][0])) {
                    logLevel[1] = (LOGLEVEL)atoi(argv[i]);

                } else {
                    return false;
                }
            }
        }
    }

    return true;
}


int main(int argc, char** argv) {

    Util::init();

    int interfaces[2] = {0, 0};

    LOGLEVEL logLevel[2] = {LEVEL_TRACE, LEVEL_ERROR};

    int componentCount[3] = {1, 1, 1};

    if (!parseParameters(argc, argv, interfaces, logLevel,
                         componentCount)) {
        PRINT("Parameter problem, exiting.....");
        return 0;
    }

    Log::init(logLevel[0], logLevel[1]);

    DeviceList *deviceList = DeviceList::getInstance();

    PRINT("Using network interfaces : %s and %s",
          deviceList->get(interfaces[0])->getName(), deviceList->get(interfaces[1])->getName());

    ComponentController* componentController = ComponentController::newInstance(interfaces);

    if (componentCount[COMP_DISTRIBUTOR]) {
        componentController->startDistributor();
    }

    if (componentCount[COMP_COLLECTOR]) {
        componentController->startCollector(componentCount[COMP_COLLECTOR]);
    }

    if (componentCount[COMP_NODE]) {
        componentController->startNode(componentCount[COMP_NODE]);
    }

    PRINT("Running in Test Mode with %d Distributor, %d Collector and %d Node",
          componentCount[COMP_DISTRIBUTOR], componentCount[COMP_COLLECTOR], componentCount[COMP_NODE]);

    PRINT("Commands : ");
    PRINT("\tPoll     : 'p' ==> Execute polling command");
    PRINT("\tLoad job : 'l' ==> Execute load job command");
    PRINT("\tProcess  : 'x' ==> Execute process command");
    PRINT("\tQuit     : 'q' ==> Exit program");

    Distributor *d = componentController->getDistributor();
    Collector *c = componentController->getCollector(0);
    Node *n = componentController->getNode(0);

    do {

        int in = getchar();
        switch(in) {
            case 'p':
                testPing(d, c, n);
                break;
            case 'c':
                testComponentList(d, c, n);
                break;
            case 's':
                testProcess(d, c, n);
                break;
            case 'i':
                testFileInfo(d, c, n);
                break;
            case 'b':
                testFileBinary(d, c, n);
                break;
            case 'j':
                testJobName(d, c, n);
                break;
            case 'q':
                return 0;
            default:
                break;
        }

    } while(true);
}
