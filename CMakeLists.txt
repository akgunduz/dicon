cmake_minimum_required(VERSION 3.3)
project(Bankor)
set(SOURCE_PATH "src")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -std=c++11")
add_definitions(-D__IDE__)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Debug/")

set(FILES
        ${SOURCE_PATH}/Application.cpp
        ${SOURCE_PATH}/Application.h
        ${SOURCE_PATH}/ArchTypes.cpp
        ${SOURCE_PATH}/ArchTypes.h
        ${SOURCE_PATH}/Collector.cpp
        ${SOURCE_PATH}/Collector.h
        ${SOURCE_PATH}/Common.cpp
        ${SOURCE_PATH}/Common.h
        ${SOURCE_PATH}/Common_apple.h
        ${SOURCE_PATH}/Common_linux.h
        ${SOURCE_PATH}/Component.cpp
        ${SOURCE_PATH}/Component.h
        ${SOURCE_PATH}/Device.cpp
        ${SOURCE_PATH}/Device.h
        ${SOURCE_PATH}/Connector.cpp
        ${SOURCE_PATH}/Connector.h
        ${SOURCE_PATH}/Console.cpp
        ${SOURCE_PATH}/Console.h
        ${SOURCE_PATH}/ConsoleClient.cpp
        ${SOURCE_PATH}/ContentItem.cpp
        ${SOURCE_PATH}/ContentItem.h
        ${SOURCE_PATH}/FileItem.cpp
        ${SOURCE_PATH}/FileItem.h
        ${SOURCE_PATH}/ParameterItem.cpp
        ${SOURCE_PATH}/ParameterItem.h
        ${SOURCE_PATH}/ExecutorItem.cpp
        ${SOURCE_PATH}/ExecutorItem.h
        ${SOURCE_PATH}/Distributor.cpp
        ${SOURCE_PATH}/Distributor.h
        ${SOURCE_PATH}/Interface.cpp
        ${SOURCE_PATH}/Interface.h
        ${SOURCE_PATH}/InterfaceCallback.cpp
        ${SOURCE_PATH}/InterfaceCallback.h
        ${SOURCE_PATH}/Log.cpp
        ${SOURCE_PATH}/Log.h
        ${SOURCE_PATH}/Job.cpp
        ${SOURCE_PATH}/Job.h
        ${SOURCE_PATH}/BaseMessage.cpp
        ${SOURCE_PATH}/BaseMessage.h
        ${SOURCE_PATH}/Message.cpp
        ${SOURCE_PATH}/Message.h
        ${SOURCE_PATH}/MessageItem.cpp
        ${SOURCE_PATH}/MessageItem.h
        ${SOURCE_PATH}/MessageDirection.cpp
        ${SOURCE_PATH}/MessageDirection.h
        ${SOURCE_PATH}/MessageTypes.cpp
        ${SOURCE_PATH}/MessageTypes.h
        ${SOURCE_PATH}/Net.cpp
        ${SOURCE_PATH}/Net.h
        ${SOURCE_PATH}/Node.cpp
        ${SOURCE_PATH}/Node.h
        ${SOURCE_PATH}/NodeItem.cpp
        ${SOURCE_PATH}/NodeItem.h
        ${SOURCE_PATH}/NodeManager.cpp
        ${SOURCE_PATH}/NodeManager.h
        ${SOURCE_PATH}/NodeWatchdog.cpp
        ${SOURCE_PATH}/NodeWatchdog.h
        ${SOURCE_PATH}/Pipe.cpp
        ${SOURCE_PATH}/Pipe.h
        ${SOURCE_PATH}/Rule.cpp
        ${SOURCE_PATH}/Rule.h
        ${SOURCE_PATH}/Scheduler.cpp
        ${SOURCE_PATH}/Scheduler.h
        ${SOURCE_PATH}/SchedulerItem.cpp
        ${SOURCE_PATH}/SchedulerItem.h
        ${SOURCE_PATH}/Util.cpp
        ${SOURCE_PATH}/Util.h
        ${SOURCE_PATH}/Unit.cpp
        ${SOURCE_PATH}/Unit.h
        ${SOURCE_PATH}/StopWatch.cpp
        ${SOURCE_PATH}/StopWatch.h
        ${SOURCE_PATH}/UnixSocket.cpp
        ${SOURCE_PATH}/UnixSocket.h
        ${SOURCE_PATH}/Address.cpp
        ${SOURCE_PATH}/Address.h
        ${SOURCE_PATH}/NetAddress.cpp
        ${SOURCE_PATH}/NetAddress.h
        ${SOURCE_PATH}/PipeAddress.cpp
        ${SOURCE_PATH}/PipeAddress.h
        ${SOURCE_PATH}/UnixSocketAddress.cpp
        ${SOURCE_PATH}/UnixSocketAddress.h
        ${SOURCE_PATH}/Job.cpp
        ${SOURCE_PATH}/Job.h
        ${SOURCE_PATH}/MessageItem.cpp
        ${SOURCE_PATH}/MessageItem.h
        ${SOURCE_PATH}/SchedulerItem.cpp
        ${SOURCE_PATH}/SchedulerItem.h
        src/JsonType.cpp
        src/JsonType.h
        src/JsonItem.cpp
        src/JsonItem.h)

set(WX_FILES
        ${SOURCE_PATH}/UserInterface.cpp
        ${SOURCE_PATH}/UserInterface.h
        ${SOURCE_PATH}/UserInterfaceApp.cpp
        ${SOURCE_PATH}/UserInterfaceApp.h
)

include_directories(${SOURCE_PATH})

if (APPLE)

    set(CMAKE_CXX_COMPILER /usr/local/bin/clang-omp++)
    #set(CMAKE_CXX_COMPILER /usr/bin/clang++)
    add_definitions(-DAPPLE -D__WXWIDGETS__)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/local/Cellar/wxmac/3.0.2/lib/wx/include/osx_cocoa-unicode-3.0 \
        -I/usr/local/Cellar/wxmac/3.0.2/include/wx-3.0 -D_FILE_OFFSET_BITS=64 \
        -DwxDEBUG_LEVEL=0 -DWXUSINGDLL -D__WXMAC__ -D__WXOSX__ -D__WXOSX_COCOA__")
    set(CMAKE_EXE_LINKER_FLAGS "-L/usr/local/Cellar/wxmac/3.0.2/lib \
        -framework IOKit -framework Carbon -framework Cocoa -framework AudioToolbox \
        -framework System -framework OpenGL -lwx_osx_cocoau_xrc-3.0 \
        -lwx_osx_cocoau_webview-3.0 -lwx_osx_cocoau_html-3.0 -lwx_osx_cocoau_qa-3.0 \
        -lwx_osx_cocoau_adv-3.0 -lwx_osx_cocoau_core-3.0 -lwx_baseu_xml-3.0 \
        -lwx_baseu_net-3.0 -lwx_baseu-3.0 -ljson-c -lcrypto")

    add_executable(bankor-osx ${FILES} ${WX_FILES})

elseif(CYGWIN)

    add_definitions(-DUNICODE -DCYGWIN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/lib/wx/include/gtk2-unicode-3.0 -I/usr/include/wx-3.0
        -D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXGTK__")

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /bin/cygjson-c-2.dll /bin/cygcrypto-1.0.0.dll
        -lwx_gtk2u_xrc-3.0 -lwx_gtk2u_webview-3.0 -lwx_gtk2u_html-3.0 -lwx_gtk2u_qa-3.0
        -lwx_gtk2u_adv-3.0 -lwx_gtk2u_core-3.0 -lwx_baseu_xml-3.0 -lwx_baseu_net-3.0 -lwx_baseu-3.0")

    add_executable(bankor-cyg ${FILES})

else()

    set(CMAKE_CXX_COMPILER /usr/bin/g++)
    add_definitions(-DLINUX -D__WXWIDGETS__)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/lib/x86_64-linux-gnu/wx/include/gtk2-unicode-3.0 \
        -I/usr/include/wx-3.0 -D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXGTK__")

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--start-group -L/usr/lib/x86_64-linux-gnu \
        -lwx_gtk2u_xrc-3.0 -lwx_gtk2u_html-3.0 -lwx_gtk2u_qa-3.0 -lwx_gtk2u_adv-3.0 -lwx_gtk2u_core-3.0 \
        -lwx_baseu_xml-3.0 -lwx_baseu_net-3.0 -lwx_baseu-3.0 -ljson-c -lssl -lcrypto -lz -pthread ")

    add_executable(bankor-lnx ${FILES} ${WX_FILES})

endif()