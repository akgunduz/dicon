cmake_minimum_required(VERSION 3.3)
project(Bankor)
set(SOURCE_PATH "src")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -march=native -mavx")

#set(CMAKE_EXE_LINKER_FLAGS " /bin/cygjson-c-2.dll -lssl -lcrypto")

if (APPLE)

    set(CMAKE_CXX_COMPILER /usr/local/bin/clang-omp++)
    add_definitions(-D__APPLE__)

elseif(CYGWIN)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/lib/wx/include/gtk2-unicode-3.0 -I/usr/include/wx-3.0
        -D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXGTK__")
    add_definitions(-DUNICODE -DCYGWIN)

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /bin/cygjson-c-2.dll /bin/cygcrypto-1.0.0.dll
        -lwx_gtk2u_xrc-3.0 -lwx_gtk2u_webview-3.0 -lwx_gtk2u_html-3.0 -lwx_gtk2u_qa-3.0
        -lwx_gtk2u_adv-3.0 -lwx_gtk2u_core-3.0 -lwx_baseu_xml-3.0 -lwx_baseu_net-3.0 -lwx_baseu-3.0")

endif()

#add_definitions(-D__WXWIDGETS__ -D__IDE__)
add_definitions(-D__IDE__)

set(FILES
        ${SOURCE_PATH}/Application.cpp
        ${SOURCE_PATH}/Application.cpp
        ${SOURCE_PATH}/Application.h
        ${SOURCE_PATH}/Client.cpp
        ${SOURCE_PATH}/Client.h
        ${SOURCE_PATH}/ClientManager.cpp
        ${SOURCE_PATH}/ClientManager.h
        ${SOURCE_PATH}/Collector.cpp
        ${SOURCE_PATH}/Collector.h
        ${SOURCE_PATH}/Common.cpp
        ${SOURCE_PATH}/Common.h
        ${SOURCE_PATH}/Common_apple.h
        ${SOURCE_PATH}/Common_linux.h
        ${SOURCE_PATH}/Connector.cpp
        ${SOURCE_PATH}/Connector.h
        ${SOURCE_PATH}/Console.cpp
        ${SOURCE_PATH}/Console.h
        ${SOURCE_PATH}/ConsoleClient.cpp
        ${SOURCE_PATH}/Content.cpp
        ${SOURCE_PATH}/Content.h
        ${SOURCE_PATH}/Distributor.cpp
        ${SOURCE_PATH}/Distributor.h
        ${SOURCE_PATH}/Interface.cpp
        ${SOURCE_PATH}/Interface.h
        ${SOURCE_PATH}/Log.cpp
        ${SOURCE_PATH}/Log.h
        ${SOURCE_PATH}/BaseMessage.cpp
        ${SOURCE_PATH}/BaseMessage.h
        ${SOURCE_PATH}/Message.cpp
        ${SOURCE_PATH}/Message.h
        ${SOURCE_PATH}/Net.cpp
        ${SOURCE_PATH}/Net.h
        ${SOURCE_PATH}/Pipe.cpp
        ${SOURCE_PATH}/Pipe.h
        ${SOURCE_PATH}/Rule.cpp
        ${SOURCE_PATH}/Rule.h
        ${SOURCE_PATH}/Scheduler.cpp
        ${SOURCE_PATH}/Scheduler.h
        ${SOURCE_PATH}/Tools.cpp
        ${SOURCE_PATH}/Tools.h
        ${SOURCE_PATH}/UnixSocket.cpp
        ${SOURCE_PATH}/UnixSocket.h
        ${SOURCE_PATH}/Address.cpp
        ${SOURCE_PATH}/Address.h
        ${SOURCE_PATH}/NetAddress.cpp
        ${SOURCE_PATH}/NetAddress.h
        ${SOURCE_PATH}/PipeAddress.cpp
        ${SOURCE_PATH}/PipeAddress.h
        ${SOURCE_PATH}/UnixSocketAddress.cpp
        ${SOURCE_PATH}/UnixSocketAddress.h)

set(WX_FILES
        ${SOURCE_PATH}/UserInterface.cpp
        ${SOURCE_PATH}/UserInterface.h
        ${SOURCE_PATH}/UserInterfaceApp.cpp
        ${SOURCE_PATH}/UserInterfaceApp.h
        ${SOURCE_PATH}/UserInterfaceClient.cpp
        ${SOURCE_PATH}/UserInterfaceCollector.cpp
        ${SOURCE_PATH}/UserInterfaceDistributor.cpp
        ${SOURCE_PATH}/UserInterfaceGenerated.cpp
        ${SOURCE_PATH}/UserInterfaceGenerated.h)

include_directories(${SOURCE_PATH})

#add_executable(Bankor ${FILES} ${WX_FILES})
add_executable(Bankor ${FILES})

#find_package(Threads REQUIRED)
#find_package(Json REQUIRED)

#find_package(wxWidgets COMPONENTS core base REQUIRED)
#target_link_libraries(Bankor ${wxWidgets_LIBRARIES})