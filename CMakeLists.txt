cmake_minimum_required(VERSION 3.3)
project(Bankor)

set(CMAKE_CXX_COMPILER /usr/local/bin/clang-omp++)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -march=native -mavx")

set(CMAKE_EXE_LINKER_FLAGS "-ljson-c")

add_definitions(-D__WXWIDGETS__ -D__APPLE__ -D__IDE__)

set(SOURCE_FILES
        src/Application.cpp
        src/Application.cpp
        src/Application.h
        src/Client.cpp
        src/Client.h
        src/ClientManager.cpp
        src/ClientManager.h
        src/Collector.cpp
        src/Collector.h
        src/Common.cpp
        src/Common.h
        src/Common_apple.h
        src/Common_linux.h
        src/Connector.cpp
        src/Connector.h
        src/Console.cpp
        src/Console.h
        src/ConsoleClient.cpp
        src/Content.cpp
        src/Content.h
        src/Distributor.cpp
        src/Distributor.h
        src/Interface.cpp
        src/Interface.h
        src/Log.cpp
        src/Log.h
        src/BaseMessage.cpp
        src/BaseMessage.h
        src/Message.cpp
        src/Message.h
        src/Net.cpp
        src/Net.h
        src/Pipe.cpp
        src/Pipe.h
        src/Rule.cpp
        src/Rule.h
        src/Scheduler.cpp
        src/Scheduler.h
        src/Tools.cpp
        src/Tools.h
        src/UnixSocket.cpp
        src/UnixSocket.h
        src/UserInterface.cpp
        src/UserInterface.h
        src/UserInterfaceApp.cpp
        src/UserInterfaceApp.h
        src/UserInterfaceClient.cpp
        src/UserInterfaceCollector.cpp
        src/UserInterfaceDistributor.cpp
        src/UserInterfaceGenerated.cpp
        src/UserInterfaceGenerated.h
        src/Address.cpp
        src/Address.h
        src/NetAddress.cpp
        src/NetAddress.h
        src/PipeAddress.cpp
        src/PipeAddress.h
        src/UnixSocketAddress.cpp
        src/UnixSocketAddress.h)

include_directories(src)

add_executable(Bankor ${SOURCE_FILES})

find_package(Threads REQUIRED)
find_package(Json REQUIRED)

find_package(wxWidgets REQUIRED)
include(${wxWidgets_USE_FILE})
target_link_libraries(Bankor ${wxWidgets_LIBRARIES})