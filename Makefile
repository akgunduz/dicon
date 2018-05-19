# Standard Makefile

SOURCE_PATH = src
EXEC_PATH = Debug
EXECUTABLE = bankor
GCC_VERSION = 4.9
TOOLCHAIN_TYPE = toolchain_arm

CFLAGS = -g -std=c++11 -ffast-math
LDFLAGS = -ljson-c -lssl -lcrypto -lz -pthread

OBJECTS_PATH = Objects

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    TOOLCHAIN_PATH = ~/Toolchains
    TOOLCHAIN_ARM_PREFIX = arm-linux-gnueabihf
else
	TOOLCHAIN_PATH = ~/Toolchains
	TOOLCHAIN_ARM_PREFIX = arm-linux-gnueabihf
endif

FILES = \
        ${SOURCE_PATH}/Application.cpp \
        ${SOURCE_PATH}/ArchTypes.cpp \
        ${SOURCE_PATH}/Collector.cpp \
        ${SOURCE_PATH}/Common.cpp \
        ${SOURCE_PATH}/Component.cpp \
        ${SOURCE_PATH}/Device.cpp \
        ${SOURCE_PATH}/Connector.cpp \
        ${SOURCE_PATH}/Console.cpp \
        ${SOURCE_PATH}/ConsoleNode.cpp \
        ${SOURCE_PATH}/ContentItem.cpp \
        ${SOURCE_PATH}/FileItem.cpp \
        ${SOURCE_PATH}/FileList.cpp \
        ${SOURCE_PATH}/ParameterItem.cpp \
        ${SOURCE_PATH}/ProcessItem.cpp \
        ${SOURCE_PATH}/Distributor.cpp \
        ${SOURCE_PATH}/Interface.cpp \
        ${SOURCE_PATH}/InterfaceCallback.cpp \
        ${SOURCE_PATH}/Log.cpp \
        ${SOURCE_PATH}/Job.cpp \
        ${SOURCE_PATH}/BaseMessage.cpp \
        ${SOURCE_PATH}/Md5.cpp \
        ${SOURCE_PATH}/Message.cpp \
        ${SOURCE_PATH}/MessageItem.cpp \
        ${SOURCE_PATH}/MessageDirection.cpp \
        ${SOURCE_PATH}/MessageTypes.cpp \
        ${SOURCE_PATH}/Net.cpp \
        ${SOURCE_PATH}/Node.cpp \
        ${SOURCE_PATH}/NodeObject.cpp \
        ${SOURCE_PATH}/NodeManager.cpp \
        ${SOURCE_PATH}/NodeWatchdog.cpp \
        ${SOURCE_PATH}/Pipe.cpp \
        ${SOURCE_PATH}/Rule.cpp \
        ${SOURCE_PATH}/Scheduler.cpp \
        ${SOURCE_PATH}/SchedulerItem.cpp \
        ${SOURCE_PATH}/MapItem.cpp \
        ${SOURCE_PATH}/Util.cpp \
        ${SOURCE_PATH}/Unit.cpp \
        ${SOURCE_PATH}/StopWatch.cpp \
        ${SOURCE_PATH}/UnixSocket.cpp \
        ${SOURCE_PATH}/AddressHelper.cpp \
        ${SOURCE_PATH}/NetAddress.cpp \
        ${SOURCE_PATH}/PipeAddress.cpp \
        ${SOURCE_PATH}/UnixSocketAddress.cpp \
        ${SOURCE_PATH}/Job.cpp \
        ${SOURCE_PATH}/MessageItem.cpp \
        ${SOURCE_PATH}/SchedulerItem.cpp \
        ${SOURCE_PATH}/JsonType.cpp \
        ${SOURCE_PATH}/JsonItem.cpp \
        ${SOURCE_PATH}/DeviceManager.cpp

WX_FILES = \
		$(SOURCE_PATH)/UserInterface.cpp \
		$(SOURCE_PATH)/UserInterfaceApp.cpp


PI_CC = $(TOOLCHAIN_PATH)/$(TOOLCHAIN_TYPE)/$(GCC_VERSION).pi/bin/$(TOOLCHAIN_ARM_PREFIX)-g++

PI_OBJECTS_PATH = Objects/pi

PI_CFLAGS = $(CFLAGS) -O2 -pipe -DARM11 -march=armv6zk -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -marm \
	-I$(TOOLCHAIN_PATH)/$(TOOLCHAIN_TYPE)/$(GCC_VERSION).pi/$(TOOLCHAIN_ARM_PREFIX)/include \
	-I$(TOOLCHAIN_PATH)/libs/pi/$(GCC_VERSION)/include

PI_LDFLAGS = -L$(TOOLCHAIN_PATH)/$(TOOLCHAIN_TYPE)/$(PI_GCC_VERSION)/$(TOOLCHAIN_ARM_PREFIX)/lib \
	-L$(TOOLCHAIN_PATH)/libs/pi/$(GCC_VERSION)/lib $(LDFLAGS)

PI_FILES = $(FILES)

PI_EXECUTABLE = $(EXECUTABLE)_pi

PI_OBJECTS = $(patsubst $(SOURCE_PATH)/%.cpp, $(PI_OBJECTS_PATH)/%.o, $(PI_FILES))

PI_DEPENDENCIES = $(patsubst $(SOURCE_PATH)/%.cpp, $(PI_OBJECTS_PATH)/%.d, $(PI_FILES))



C15_CC = $(TOOLCHAIN_PATH)/$(TOOLCHAIN_TYPE)/$(GCC_VERSION)/bin/$(TOOLCHAIN_ARM_PREFIX)-g++

C15_OBJECTS_PATH = Objects/c15

C15_CFLAGS = $(CFLAGS) -O3 -DCORTEXA15 -march=armv7ve -mcpu=cortex-a15 -mfloat-abi=hard -mfpu=neon \
	-mvectorize-with-neon-quad -I$(TOOLCHAIN_PATH)/$(TOOLCHAIN_TYPE)/$(GCC_VERSION)/$(TOOLCHAIN_ARM_PREFIX)/include \
	-I$(TOOLCHAIN_PATH)/libs/c15/$(GCC_VERSION)/include

C15_LDFLAGS = -L$(TOOLCHAIN_PATH)/$(TOOLCHAIN_TYPE)/$(GCC_VERSION)/$(TOOLCHAIN_ARM_PREFIX)/lib \
	-L$(TOOLCHAIN_PATH)/libs/c15/$(GCC_VERSION)/lib $(LDFLAGS) 

C15_FILES = $(FILES)

C15_EXECUTABLE = $(EXECUTABLE)_c15

C15_OBJECTS = $(patsubst $(SOURCE_PATH)/%.cpp, $(C15_OBJECTS_PATH)/%.o, $(C15_FILES))

C15_DEPENDENCIES = $(patsubst $(SOURCE_PATH)/%.cpp, $(C15_OBJECTS_PATH)/%.d, $(C15_FILES))


C9_CC = $(TOOLCHAIN_PATH)/$(TOOLCHAIN_TYPE)/$(GCC_VERSION)/bin/$(TOOLCHAIN_ARM_PREFIX)-g++

C9_OBJECTS_PATH = Objects/c9

C9_CFLAGS = $(CFLAGS) -O3 -DCORTEXA9 -march=armv7-a -mcpu=cortex-a9 -mfloat-abi=hard -mfpu=neon \
	-mvectorize-with-neon-quad -I$(TOOLCHAIN_PATH)/$(TOOLCHAIN_TYPE)/$(GCC_VERSION)/$(TOOLCHAIN_ARM_PREFIX)/include \
	-I$(TOOLCHAIN_PATH)/libs/c9/$(GCC_VERSION)/include

C9_LDFLAGS = -L$(TOOLCHAIN_PATH)/$(TOOLCHAIN_TYPE)/$(GCC_VERSION)/$(TOOLCHAIN_ARM_PREFIX)/lib \
	-L$(TOOLCHAIN_PATH)/libs/c9/$(GCC_VERSION)/lib $(LDFLAGS) 

C9_FILES = $(FILES)

C9_EXECUTABLE = $(EXECUTABLE)_c9

C9_OBJECTS = $(patsubst $(SOURCE_PATH)/%.cpp, $(C9_OBJECTS_PATH)/%.o, $(C9_FILES))

C9_DEPENDENCIES = $(patsubst $(SOURCE_PATH)/%.cpp, $(C9_OBJECTS_PATH)/%.d, $(C9_FILES))



C8_CC = $(TOOLCHAIN_PATH)/$(TOOLCHAIN_TYPE)/$(GCC_VERSION)/bin/$(TOOLCHAIN_ARM_PREFIX)-g++

C8_OBJECTS_PATH = Objects/c8

C8_CFLAGS = $(CFLAGS) -O3 -DCORTEXA8 -march=armv7-a -mcpu=cortex-a8 -mfloat-abi=hard -mfpu=neon \
	-mvectorize-with-neon-quad -I$(TOOLCHAIN_PATH)/$(TOOLCHAIN_TYPE)/$(GCC_VERSION)/$(TOOLCHAIN_ARM_PREFIX)/include \
	-I$(TOOLCHAIN_PATH)/libs/c8/$(GCC_VERSION)/include

C8_LDFLAGS = -L$(TOOLCHAIN_PATH)/$(TOOLCHAIN_TYPE)/$(GCC_VERSION)/$(TOOLCHAIN_ARM_PREFIX)/lib \
	-L$(TOOLCHAIN_PATH)/libs/c8/$(GCC_VERSION)/lib $(LDFLAGS) 

C8_FILES = $(FILES)

C8_EXECUTABLE = $(EXECUTABLE)_c8

C8_OBJECTS = $(patsubst $(SOURCE_PATH)/%.cpp, $(C8_OBJECTS_PATH)/%.o, $(C8_FILES))

C8_DEPENDENCIES = $(patsubst $(SOURCE_PATH)/%.cpp, $(C8_OBJECTS_PATH)/%.d, $(C8_FILES))



C7_CC = $(TOOLCHAIN_PATH)/$(TOOLCHAIN_TYPE)/$(GCC_VERSION)/bin/$(TOOLCHAIN_ARM_PREFIX)-g++

C7_OBJECTS_PATH = Objects/c7

C7_CFLAGS = $(CFLAGS) -O3 -DCORTEXA7 -mcpu=cortex-a7 -mfloat-abi=hard -mfpu=neon-vfpv4 \
	-mvectorize-with-neon-quad -I$(TOOLCHAIN_PATH)/$(TOOLCHAIN_TYPE)/$(GCC_VERSION)/$(TOOLCHAIN_ARM_PREFIX)/include \
	-I$(TOOLCHAIN_PATH)/libs/c7/$(GCC_VERSION)/include

C7_LDFLAGS = -L$(TOOLCHAIN_PATH)/$(TOOLCHAIN_TYPE)/$(GCC_VERSION)/$(TOOLCHAIN_ARM_PREFIX)/lib \
	-L$(TOOLCHAIN_PATH)/libs/c7/$(GCC_VERSION)/lib $(LDFLAGS) 

C7_FILES = $(FILES)

C7_EXECUTABLE = $(EXECUTABLE)_c7

C7_OBJECTS = $(patsubst $(SOURCE_PATH)/%.cpp, $(C7_OBJECTS_PATH)/%.o, $(C7_FILES))

C7_DEPENDENCIES = $(patsubst $(SOURCE_PATH)/%.cpp, $(C7_OBJECTS_PATH)/%.d, $(C7_FILES))


ARM_CC = $(TOOLCHAIN_PATH)/$(TOOLCHAIN_TYPE)/$(GCC_VERSION)/bin/$(TOOLCHAIN_ARM_PREFIX)-g++

ARM_OBJECTS_PATH = Objects/arm

ARM_CFLAGS = $(CFLAGS) -O3 -mfloat-abi=hard -mfpu=neon \
	-mvectorize-with-neon-quad -I$(TOOLCHAIN_PATH)/$(TOOLCHAIN_TYPE)/$(GCC_VERSION)/$(TOOLCHAIN_ARM_PREFIX)/include \
	-I$(TOOLCHAIN_PATH)/libs/arm/$(GCC_VERSION)/include

ARM_LDFLAGS = -L$(TOOLCHAIN_PATH)/$(TOOLCHAIN_TYPE)/$(GCC_VERSION)/$(TOOLCHAIN_ARM_PREFIX)/lib \
	-L$(TOOLCHAIN_PATH)/libs/arm/$(GCC_VERSION)/lib $(LDFLAGS) 

ARM_FILES = $(FILES)

ARM_EXECUTABLE = $(EXECUTABLE)_arm

ARM_OBJECTS = $(patsubst $(SOURCE_PATH)/%.cpp, $(ARM_OBJECTS_PATH)/%.o, $(ARM_FILES))

ARM_DEPENDENCIES = $(patsubst $(SOURCE_PATH)/%.cpp, $(ARM_OBJECTS_PATH)/%.d, $(ARM_FILES))


CMD_CC = g++

CMD_OBJECTS_PATH = Objects/cmd

CMD_CFLAGS = $(CFLAGS) -O3 -DLINUX -pipe

CMD_LDFLAGS = -L/usr/local/lib $(LDFLAGS)

CMD_FILES = $(FILES)

CMD_EXECUTABLE = $(EXECUTABLE)_cmd

CMD_OBJECTS = $(patsubst $(SOURCE_PATH)/%.cpp, $(CMD_OBJECTS_PATH)/%.o, $(CMD_FILES))

CMD_DEPENDENCIES = $(patsubst $(SOURCE_PATH)/%.cpp, $(CMD_OBJECTS_PATH)/%.d, $(CMD_FILES))



UI_CC = /usr/local/bin/clang-omp++

UI_OBJECTS_PATH = Objects/ui

UI_CFLAGS = $(CFLAGS) -O3 -pipe -DAPPLE -D__WXWIDGETS__ -D_FILE_OFFSET_BITS=64 -DwxDEBUG_LEVEL=0 -DWXUSINGDLL -D__WXMAC__ -D__WXOSX__ -D__WXOSX_COCOA__ \
	-I/usr/local/include/wx-3.0 -I/usr/local/lib/wx/include/osx_cocoa-unicode-3.0 

UI_LDFLAGS = -L/usr/local/lib -framework IOKit -framework Carbon \
	-framework Cocoa -lwx_osx_cocoau_xrc-3.0 -lwx_osx_cocoau_webview-3.0 \
	-lwx_osx_cocoau_html-3.0 -lwx_osx_cocoau_qa-3.0 -lwx_osx_cocoau_adv-3.0 \
	-lwx_osx_cocoau_core-3.0 -lwx_baseu_xml-3.0 -lwx_baseu_net-3.0 -lwx_baseu-3.0 $(LDFLAGS)

UI_FILES = $(FILES) $(WX_FILES)

UI_EXECUTABLE = $(EXECUTABLE)_ui

UI_OBJECTS = $(patsubst $(SOURCE_PATH)/%.cpp, $(UI_OBJECTS_PATH)/%.o, $(UI_FILES))

UI_DEPENDENCIES = $(patsubst $(SOURCE_PATH)/%.cpp, $(UI_OBJECTS_PATH)/%.d, $(UI_FILES))




UILNX_CC = g++

UILNX_OBJECTS_PATH = Objects/uilnx

UILNX_CFLAGS = $(CFLAGS) -O0 -pipe -DLINUX -D__WXWIDGETS__ -I/usr/lib/x86_64-linux-gnu/wx/include/gtk2-unicode-3.0 \
    -I/usr/include/wx-3.0 -D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXGTK__

UILNX_LDFLAGS = -L/usr/lib/x86_64-linux-gnu  -lwx_gtk2u_xrc-3.0 -lwx_gtk2u_html-3.0 -lwx_gtk2u_qa-3.0 \
    -lwx_gtk2u_adv-3.0 -lwx_gtk2u_core-3.0 -lwx_baseu_xml-3.0 -lwx_baseu_net-3.0 -lwx_baseu-3.0 $(LDFLAGS)

UILNX_FILES = $(FILES) $(WX_FILES)

UILNX_EXECUTABLE = $(EXECUTABLE)_uilnx

UILNX_OBJECTS = $(patsubst $(SOURCE_PATH)/%.cpp, $(UILNX_OBJECTS_PATH)/%.o, $(UILNX_FILES))

UILNX_DEPENDENCIES = $(patsubst $(SOURCE_PATH)/%.cpp, $(UILNX_OBJECTS_PATH)/%.d, $(UILNX_FILES))


#CFLAGS=-Os -g -fomit-frame-pointer -fno-common -fno-builtin -Wall -Dlinux -D__linux__ -Dunix -D__uClinux__ -D__linux -DEMBED -DNO_FORK
#LDFLAGS=-Wl,-elf2flt -g -static -Wl,--relax


all: uilnx

$(EXEC_PATH):
	@mkdir -p $@



$(UI_EXECUTABLE): $(UI_OBJECTS)
	$(UI_CC) -o $(EXEC_PATH)/$@ $^ $(UI_LDFLAGS)

$(UI_OBJECTS_PATH)/%.d: $(SOURCE_PATH)/%.cpp
	$(UI_CC) $(UI_CFLAGS) -MM -MT $(UI_OBJECTS_PATH)/$*.o $< >> $@

$(UI_OBJECTS_PATH)/%.o: $(SOURCE_PATH)/%.cpp
	$(UI_CC) $(UI_CFLAGS) -c -o $@ $<

$(UI_OBJECTS_PATH):
	@mkdir -p $@

ui: $(UI_OBJECTS_PATH) $(EXEC_PATH) $(UI_EXECUTABLE) $(UI_DEPENDENCIES)


$(UILNX_EXECUTABLE): $(UILNX_OBJECTS)
	$(UILNX_CC) -o $(EXEC_PATH)/$@ $^ $(UILNX_LDFLAGS)

$(UILNX_OBJECTS_PATH)/%.d: $(SOURCE_PATH)/%.cpp
	$(UILNX_CC) $(UILNX_CFLAGS) -MM -MT $(UILNX_OBJECTS_PATH)/$*.o $< >> $@

$(UILNX_OBJECTS_PATH)/%.o: $(SOURCE_PATH)/%.cpp
	$(UILNX_CC) $(UILNX_CFLAGS) -c -o $@ $<

$(UILNX_OBJECTS_PATH):
	@mkdir -p $@

uilnx: $(UILNX_OBJECTS_PATH) $(EXEC_PATH) $(UILNX_EXECUTABLE) $(UILNX_DEPENDENCIES)



$(CMD_EXECUTABLE): $(CMD_OBJECTS)
	$(CMD_CC) -o $(EXEC_PATH)/$@ $^ $(CMD_LDFLAGS) 

$(CMD_OBJECTS_PATH)/%.d: $(SOURCE_PATH)/%.cpp
	$(CMD_CC) $(CMD_CFLAGS) -MM -MT $(CMD_OBJECTS_PATH)/$*.o $< >> $@

$(CMD_OBJECTS_PATH)/%.o: $(SOURCE_PATH)/%.cpp
	$(CMD_CC) $(CMD_CFLAGS) -c -o $@ $<

$(CMD_OBJECTS_PATH):
	@mkdir -p $@

cmd: $(CMD_OBJECTS_PATH) $(EXEC_PATH) $(CMD_EXECUTABLE) $(CMD_DEPENDENCIES)



$(PI_EXECUTABLE): $(PI_OBJECTS)
	$(PI_CC) -o $(EXEC_PATH)/$@ $^ $(PI_LDFLAGS) 

$(PI_OBJECTS_PATH)/%.d: $(SOURCE_PATH)/%.cpp
	$(PI_CC) $(PI_CFLAGS) -MM -MT $(PI_OBJECTS_PATH)/$*.o $< >> $@

$(PI_OBJECTS_PATH)/%.o: $(SOURCE_PATH)/%.cpp
	$(PI_CC) $(PI_CFLAGS) -c -o $@ $<

$(PI_OBJECTS_PATH):
	@mkdir -p $@

pi: $(PI_OBJECTS_PATH) $(EXEC_PATH) $(PI_EXECUTABLE) $(PI_DEPENDENCIES)



$(C15_EXECUTABLE): $(C15_OBJECTS)
	$(C15_CC) -o $(EXEC_PATH)/$@ $^ $(C15_LDFLAGS) 

$(C15_OBJECTS_PATH)/%.d: $(SOURCE_PATH)/%.cpp
	$(C15_CC) $(C15_CFLAGS) -MM -MT $(C15_OBJECTS_PATH)/$*.o $< >> $@

$(C15_OBJECTS_PATH)/%.o: $(SOURCE_PATH)/%.cpp
	$(C15_CC) $(C15_CFLAGS) -c -o $@ $<

$(C15_OBJECTS_PATH):
	@mkdir -p $@

c15: $(C15_OBJECTS_PATH) $(EXEC_PATH) $(C15_EXECUTABLE) $(C15_DEPENDENCIES)



$(C9_EXECUTABLE): $(C9_OBJECTS)
	$(C9_CC) -o $(EXEC_PATH)/$@ $^ $(C9_LDFLAGS) 

$(C9_OBJECTS_PATH)/%.d: $(SOURCE_PATH)/%.cpp
	$(C9_CC) $(C9_CFLAGS) -MM -MT $(C9_OBJECTS_PATH)/$*.o $< >> $@

$(C9_OBJECTS_PATH)/%.o: $(SOURCE_PATH)/%.cpp
	$(C9_CC) $(C9_CFLAGS) -c -o $@ $<

$(C9_OBJECTS_PATH):
	@mkdir -p $@

c9: $(C9_OBJECTS_PATH) $(EXEC_PATH) $(C9_EXECUTABLE) $(C9_DEPENDENCIES)



$(C8_EXECUTABLE): $(C8_OBJECTS)
	$(C8_CC) -o $(EXEC_PATH)/$@ $^ $(C8_LDFLAGS) 

$(C8_OBJECTS_PATH)/%.d: $(SOURCE_PATH)/%.cpp
	$(C8_CC) $(C8_CFLAGS) -MM -MT $(C8_OBJECTS_PATH)/$*.o $< >> $@

$(C8_OBJECTS_PATH)/%.o: $(SOURCE_PATH)/%.cpp
	$(C8_CC) $(C8_CFLAGS) -c -o $@ $<

$(C8_OBJECTS_PATH):
	@mkdir -p $@

c8: $(C8_OBJECTS_PATH) $(EXEC_PATH) $(C8_EXECUTABLE) $(C8_DEPENDENCIES)



$(C7_EXECUTABLE): $(C7_OBJECTS)
	$(C7_CC) -o $(EXEC_PATH)/$@ $^ $(C7_LDFLAGS) 

$(C7_OBJECTS_PATH)/%.d: $(SOURCE_PATH)/%.cpp
	$(C7_CC) $(C7_CFLAGS) -MM -MT $(C7_OBJECTS_PATH)/$*.o $< >> $@

$(C7_OBJECTS_PATH)/%.o: $(SOURCE_PATH)/%.cpp
	$(C7_CC) $(C7_CFLAGS) -c -o $@ $<

$(C7_OBJECTS_PATH):
	@mkdir -p $@

c7: $(C7_OBJECTS_PATH) $(EXEC_PATH) $(C7_EXECUTABLE) $(C7_DEPENDENCIES)


$(ARM_EXECUTABLE): $(ARM_OBJECTS)
	$(ARM_CC) -o $(EXEC_PATH)/$@ $^ $(ARM_LDFLAGS) 

$(ARM_OBJECTS_PATH)/%.d: $(SOURCE_PATH)/%.cpp
	$(ARM_CC) $(ARM_CFLAGS) -MM -MT $(ARM_OBJECTS_PATH)/$*.o $< >> $@

$(ARM_OBJECTS_PATH)/%.o: $(SOURCE_PATH)/%.cpp
	$(ARM_CC) $(ARM_CFLAGS) -c -o $@ $<

$(ARM_OBJECTS_PATH):
	@mkdir -p $@

arm: $(ARM_OBJECTS_PATH) $(EXEC_PATH) $(ARM_EXECUTABLE) $(ARM_DEPENDENCIES)

install:

clean:
	-rm -f -R $(OBJECTS_PATH)
