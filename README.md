# Dicon - An HPC architecture for embedded systems

The goal of this architecture is to divide a large-scale application into small chunks of work that are executed on embedded computers. The advantage of the system is that the increased number of computers that constitute the system helps distribute the risks associated with system sustainability across a larger number of sub-systems, improving the overall system reliability, performance and efficiency.

The architecture is designed as part of a master thesis for academical purposes, however it is applicable for any kind of distributed task driven projects using low cost embedded devices.

Development cycle has still open points and needs substantial improvements to cover all corner cases, So all contributions and suggestions in architectural design and help in development are welcome :)

### Quick Overview

Architecture's main operation is based on collecting a target large-scale applications subtasks (processes) and distribute them to available embedded computers to be processed. In order to achieve that, first the application itself should be analyzed and divide into processes. At this point a rule based structure is needed to make the architecture transparent to the target application. In this way application creator is the main responsable for defining the rules and decompose the target application to subtasks. Analyze process step includes dependency tracking functionality based on **Kahn's** algorithm and it creates dependency map of the rules that depends each other.

The architecture consist of three main modules; **Distributor, Collector(s), Node(s)**

- **Distributor** Keeps track of the collectors and nodes lifecycles and their availability. However it does not involve in any act related to the process going between collectors and nodes.

- **Collectors** Analyze the applications and their subtasks based on their predefined rule files which defines the subtask process details. Then it transfers the subtasks to available nodes to be executed.

- **Nodes** Runs the application subtasks that comes from collectors as defined in their rule files.

![alt text](docs/DomainModel.png)

### Requirements

System can build on **Linux**, **macOS** and **Windows (via WSL 2)** operating systems using **gcc**, **clang** vs toolchains to produce application executables. CMake and GNU make is used as compile platform which have wide range of usage.

[**Civetweb**](https://github.com/civetweb/civetweb) library is selected for  application's UI. Its main advantage is to serve the UI in a web page, which allows to access the UI even from embedded devices. It provide cross-platform support and written in C++ to have high response and wide range support.

[**Json-c**](https://github.com/json-c/json-c) library is selected for json-parsing, used as job description interface.

### Getting Started 

The architecture source code is bundled with the UI code, which will be seperated in the future. But for now, it simplifies the whole compile process in one step. 

#### Compilation

Compile process is carried out with **CMake** build system and **gcc** or **clang** toolchains.

On **MacOS** Platform;

if not installed already, install **HomeBrew** package system

```
	ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

Install build utilities

```
	brew install cmake
	brew install g++
```

Install dependencies

```
	brew install json-c
```

To cross-compile for linux based nodes; get sysroot enabled toolchains; ***arm-linux-gnueabihf***, ***x86-linux-gnueabihf*** and put them under ***/usr/local/toolchains*** or update the **TOOLCHAIN_DIR** variable in the corresponding toolchain cmake files.

On **Linux** and **Windows (WSL 2)** Platform;

```
	apt install json-c
```


Create build directory

```
	mkdir build
	cd build
```
Build process

- for target ==> MacOS, Linux, Windows

```
	cmake ..
	cmake --build .
```

- for target ==> ARM based Linux

```
	cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain-linux-arm.cmake
	cmake --build . --target dicon
```

- for target ==> x86 based Linux

```
	cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain-linux-x86.cmake
	cmake --build . --target dicon
```

#### Locating Target Files

Application gets the target job files as a single compressed zip file through Web UI and extracts on the server. As soon as operator triggers the process command via Collector, system parse the job file and starts the distributed execution process across the node devices.

### Workflow

The workflow of the whole process starts at computers running collectors and finishes at the nodes depicted as follows;

1. Distributor sends **Wakeup** broadcast to the network, and keeps it on with specific intervals to dynamically add and remove nodes from the system.
2. All the nodes and collectors get wakeup message, response to distributor with **Alive** messages.
3. Distributor marks the all nodes that get response as **Available**. If it does not get a response from a previously marked node for a specific amount of time, it is removed from the system.
4. Users define the applications/subtasks to the system with the user interface provided by collectors and trig the whole process.
5. Collector picks a rule from the rule set to be processed and  sends a **Node** message to distributor to request a available node.
6. Distributor gets the node message from the collector and search for available nodes in the node list. If found, it marks the nodes with **pre-busy** and sends the address of the nodes to the collector with **Node** message. Then it, waits for **Busy** message from the node. If not found, it sends a **Node** message to collector with no available node info.
7. Collector gets the node message from distributor, if there is no available node then it waits in message waiting state. If there is, then it analyzes the rule and sends it to the nodes  with **Process** message
8. Node gets the **Process** message and immediately sends a **Busy** message to distributor to inform the distributor about its state. 
9. Distributor sends **Process** message to Node to start the process.
10. Node parse the rule information and extracts all dependencies. If dependencies includes files then it searches for this files in its file system. If any of them is not found, it sends a **Info** message to Collector to request these missing files.
11. Collector gets the **Info** message and checks the file informations with the ones in the rule file. If found, it sends them back to Node with **Binary** message.
12. Node gets the **Binary** message, and place the files to the appropriate directories as depicted in the rule file. Then runs the process executable listed in the rule file via shell of the host OS and wait its finish. Then it sends the generated output files to Collector with **Binary** message.
13. Collector gets the **Binary**  message and sends back **Ready** message to Node.
14. Node sends **Ready** message to Distributor and makes its state back to IDLE.
15. Meanwhile, Collector updates the job dependencies and retrigger the whole cycle with the remaining processes.

![alt text](docs/workflow.png)


### Messaging

The messaging between components designed with socket based architecture, internet sockets and unix sockets. They can be used interchangeably however unix sockets can only be used if the components that the unix sockets selected as messaging interface must be run in the same computer/node. Message objects created with block structure to ease the serialize/deserialize process. Every message has below structure;

![alt text](docs/messageblocks.png)

- **Signature** field is used ensure the syncronization handled successfully in case of any socket buffer related problems. It is just a random 16bit number. If it is not matched in both sender and receiver side, the message is disgarded.

- **Header** field includes component specific fields to ease the decomposition of the message based on component type.


![alt text](docs/messageheader.png)

- Undefined number of **block** fields includes different kinds of contents, helps to extract messages in object oriented manner. There are six types of blocks with different count of sub-blocks;

	- **File Information Block**: Includes referencial place in the file system and MD5 information
	- **File Content Block**: Includes referencial place in the file system, MD5 information and file content
	- **MD5 Block**: Includes MD5 information
	- **Variant Block**: Includes variant type and content
	- **Executor Block**: Includes executor macros
	- **End Block**: Specifies the end of the block chain. It helps the component to create the whole message to be used in the process.

![alt text](docs/messageblockheader.png)

#### Communication Interface

The communication interface is structured based on sockets and it includes messaging related common tasks which are summarized as;

- Creates the messaging channels seperate for send and receive.
- Creates a queue mechanism to hold and sort all send/receive messages to be executed based on their priorities.
- Setups all message  execution callbacks
- Finalize the messaging mechanism
- Creates the notification related tasks


#### Message Transfer Flow

![alt text](docs/messageflow.png)

### Jobs

The subtasks of the applications that are going to execute in nodes are defined in job files.  Each job has its own directory starting with **"Job_"** prefix. In Job directories all the task specific files and job files are located in their predefined locations. JSON file format is choosed to define the whole structure of job file. Basic operation can be simplied as; the user initiates the execution process through user interface of collector, then collector loads the **"Job.json"** file resides in the application folder which is selected through user interface. (Currently fixed to directory starts with Job_ prefix, near to application service).

Job files includes all of the dependencies and process details of the subtasks in order to execute properly.

There are four types of contents that can be defined in job files;

- **Name Content** It contains the name of the job;

- **File Content** It includes the necessary file information that are going to be used by subtasks in the nodes. First field defines the name of the file and second field reserved for future use, currently only common type is supported. It defines with files tag in the json file. The related parameters are;

	- **"c"** : common file

- **Parameter Content** It includes the parameter sets that are going to pass to executable files; It defines with "parameters" tag in the json file.

- **Executable Content** It contains the command sequence of the executable list that is going to be run in nodes. Basically it is defined as macro which has references to file and parameter contents.  It defines with "executors" tag in the json file. The structure of the macro is;

		"$(F/P)INDEX"



	The definition of the macro items are;
	
	- **"$"** : Start of the macro
	- **"F"** : Reference to the file list in the rule file
	- **"P"** : Reference to the parameter list in the rule file
	- **"INDEX"** : Index of the corresponding list


Sample  Job.json file is as follows;

        "Job": {
    
            "name": "Test",
    
            "files": [
                ["benchpar", "c"],
                ["matrix/MatrixInput_1", "c"],
                ["matrix/MatrixInput_2", "c"],
                ["matrix/MatrixInput_3", "c"],
                ["matrix/MatrixInput_512_512", "c"],
            ],
    
            "parameters": [
                "10"
            ],
    
            "executors": [
                "$F0 -a m -m 2 -r $P0 $F4",
                "$F0 -a m -m 4 -r $P0 $F1",
            ]
    	}

In this sample the subtask runs following commands sequentially;

```sh
$ benchpar  -a m -m 2 -r 10 matrix/MatrixInput_512_512
$ benchpar  -a m -m 2 -r 10 matrix/MatrixInput_1
```

### User Interface

There are two different types of user interface is designed to test the architecture. First one is based on **wxWidgets** library which is a platform independent C++ based UI library, the second is works interactively through terminal.

Distributor and Collector needs to run on a standard computer with wxwidget based user interface, however Nodes can run on both user interfaces.

To test the whole architecture, all distributor, collectors and nodes should be initialized. If they are going to test with single UI, then all the initialize actions should be triggered in all tabs. Address bind operation is based on the availability of the ethernet NICs and unix sockets. If there is limited number of NIC is available then differentiation is handled with selecting different port numbers automatically.

After distributor is initialized, it sends broadcast messages for checking the collectors and nodes in the environment, and for specific amount of time it repeats the process. You can also manually trig the polling devices step with **Poll** button through UI.

![alt text](docs/dist-ui.png)

On Collector side, if it connects to a distributor, it allows users to run their jobs via its UI. Similat to distributor, it is also needed to initialize Collector via UI to bind socket interface. Then it is possible to return Distributors wakeup messages. If there is an attachment established with a distributor then the whole execution process on nodes can be triggered via **Process** button throuh UI.

![alt text](docs/coll-ui.png)

Finally on Node side, after the initialization is done, node get interacts with Collectors requests to run the subtask through shell.

![alt text](docs/node-ui.png)

Since the UI is designed only for test & demo purposes, the main progress can be shown with UI elements especially through **Log** window at the bottom.

### Open Issues

- Project is in still early development stage therefore lots of corner cases and scenarios should be tested.
- Demo application is developed just for testing the architecture purpose and can only be used as a reference; it is not suitable to be used in a final product.
- After the nodes finish the assigned process, there is no feedback or collection of results mechanism; ie subtasks are all on their own like uploading the results somewhere else etc. Distributor & Collector only gets job is done feedback from the nodes, thats all.
- UI will be replaced with webserver based framework.

### Contributing

You are welcome to contribute to this project in all manner, involving in the development, making suggestions, notifying of wrong or missing design related parts or testing.

### License

Project is licensed under [GNU Affero General Public License v3.0](LICENSE)
