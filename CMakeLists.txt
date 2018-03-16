cmake_minimum_required(VERSION 3.11)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if (DEFINED ARM)
        set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/toolchain-linux-arm.cmake")
    elseif(APPLE)
        set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/toolchain-apple-x86.cmake")
    elseif(UNIX)
        #set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/toolchain-linux-x86.cmake")
    elseif(WIN32)
        set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/toolchain-windows-x86.cmake")
    else()
        message(FATAL_ERROR "Your platform does not supported!!!")
    endif()
endif()

set(TARGET_SUFFIX lnx)

project(Bankor)

set(SOURCE_PATH "src")

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_VERBOSE_MAKEFILE 1)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -std=c++11")

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Debug/")

set(FRAMEWORK_FILES
        ${SOURCE_PATH}/ArchTypes.cpp
        ${SOURCE_PATH}/Collector.cpp
        ${SOURCE_PATH}/Common.cpp
        ${SOURCE_PATH}/Component.cpp
        ${SOURCE_PATH}/Device.cpp
        ${SOURCE_PATH}/Connector.cpp
        ${SOURCE_PATH}/ContentItem.cpp
        ${SOURCE_PATH}/FileItem.cpp
        ${SOURCE_PATH}/FileList.cpp
        ${SOURCE_PATH}/ParameterItem.cpp
        ${SOURCE_PATH}/ExecutorItem.cpp
        ${SOURCE_PATH}/DeviceManager.cpp
        ${SOURCE_PATH}/Distributor.cpp
        ${SOURCE_PATH}/Interface.cpp
        ${SOURCE_PATH}/InterfaceCallback.cpp
        ${SOURCE_PATH}/Log.cpp
        ${SOURCE_PATH}/Job.cpp
        ${SOURCE_PATH}/BaseMessage.cpp
        ${SOURCE_PATH}/Md5.cpp
        ${SOURCE_PATH}/Message.cpp
        ${SOURCE_PATH}/MessageItem.cpp
        ${SOURCE_PATH}/MessageDirection.cpp
        ${SOURCE_PATH}/MessageTypes.cpp
        ${SOURCE_PATH}/Net.cpp
        ${SOURCE_PATH}/Node.cpp
        ${SOURCE_PATH}/NodeItem.cpp
        ${SOURCE_PATH}/NodeManager.cpp
        ${SOURCE_PATH}/NodeWatchdog.cpp
        ${SOURCE_PATH}/Rule.cpp
        ${SOURCE_PATH}/Scheduler.cpp
        ${SOURCE_PATH}/SchedulerItem.cpp
        ${SOURCE_PATH}/MapItem.cpp
        ${SOURCE_PATH}/Util.cpp
        ${SOURCE_PATH}/Unit.cpp
        ${SOURCE_PATH}/StopWatch.cpp
        ${SOURCE_PATH}/UnixSocket.cpp
        ${SOURCE_PATH}/Address.cpp
        ${SOURCE_PATH}/NetAddress.cpp
        ${SOURCE_PATH}/PipeAddress.cpp
        ${SOURCE_PATH}/UnixSocketAddress.cpp
        ${SOURCE_PATH}/Job.cpp
        ${SOURCE_PATH}/MessageItem.cpp
        ${SOURCE_PATH}/SchedulerItem.cpp
        ${SOURCE_PATH}/JsonType.cpp
        ${SOURCE_PATH}/JsonItem.cpp
)

set(WX_FILES
        ${SOURCE_PATH}/ui/wx/UserInterface.cpp
        ${SOURCE_PATH}/ui/wx/UserInterfaceDistributor.cpp
        ${SOURCE_PATH}/ui/wx/UserInterfaceCollector.cpp
        ${SOURCE_PATH}/ui/wx/UserInterfaceNode.cpp
        ${SOURCE_PATH}/ui/wx/UserInterfaceApp.cpp
)

set(CONSOLE_FILES
        ${SOURCE_PATH}/ui/console/Console.cpp
        ${SOURCE_PATH}/ui/console/ConsoleApp.cpp
        ${SOURCE_PATH}/ui/console/ConsoleNode.cpp
)

find_package(PkgConfig)
pkg_check_modules(JSON-C REQUIRED QUIET json-c)
find_package(OpenSSL)
find_package(wxWidgets)

message("------${JSON-C_FOUND}")
message("------${JSON-C_INCLUDE_DIRS}")
message("------${JSON-C_LIBRARIES}")

message("------${OPENSSL_FOUND}")
message("------${OPENSSL_INCLUDE_DIR}")
message("------${OPENSSL_CRYPTO_LIBRARY}")

message("------${wxWidgets_FOUND}")
message("------${wxWidgets_CXX_FLAGS}")
message("------${wxWidgets_DEFINITIONS}")
message("------${wxWidgets_INCLUDE_DIRS}")
message("------${wxWidgets_LIBRARIES}")

#add_definitions(-D__WXGTK__)

include_directories(${SOURCE_PATH})
include_directories(${JSON-C_INCLUDE_DIRS})
#include_directories(${OPENSSL_INCLUDE_DIR})


#include_directories(${wxWidgets_INCLUDE_DIRS})

#add_definitions(-DLINUX -D__WXWIDGETS__)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/lib/x86_64-linux-gnu/wx/include/gtk2-unicode-3.0 \
#    -I/usr/include/wx-3.0 -D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXGTK__")
#
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--start-group -L/usr/lib/x86_64-linux-gnu \
#    -lwx_gtk2u_xrc-3.0 -lwx_gtk2u_html-3.0 -lwx_gtk2u_qa-3.0 -lwx_gtk2u_adv-3.0 -lwx_gtk2u_core-3.0 \
#    -lwx_baseu_xml-3.0 -lwx_baseu_net-3.0 -lwx_baseu-3.0 -ljson-c -lssl -lcrypto -lz -pthread ")

add_library(bankor-framework-${TARGET_SUFFIX} ${FRAMEWORK_FILES})
target_include_directories(bankor-framework-${TARGET_SUFFIX} PUBLIC ${OPENSSL_INCLUDE_DIR})

add_executable(bankor-wx-${TARGET_SUFFIX} ${WX_FILES})
target_compile_definitions(bankor-wx-${TARGET_SUFFIX} PUBLIC ${wxWidgets_DEFINITIONS})
target_include_directories(bankor-wx-${TARGET_SUFFIX} PUBLIC ${wxWidgets_INCLUDE_DIRS})
target_compile_options(bankor-wx-${TARGET_SUFFIX} PUBLIC ${wxWidgets_CXX_FLAGS})

target_link_libraries(bankor-wx-${TARGET_SUFFIX} bankor-framework-${TARGET_SUFFIX}
        ${JSON-C_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARY} ${wxWidgets_LIBRARIES} )

add_executable(bankor-console-${TARGET_SUFFIX} ${CONSOLE_FILES})

target_link_libraries(bankor-console-${TARGET_SUFFIX} bankor-framework-${TARGET_SUFFIX}
        ${JSON-C_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARY} pthread)



