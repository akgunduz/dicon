cmake_minimum_required(VERSION 3.9)

project(Bankor)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_VERBOSE_MAKEFILE 1)

set(SOURCE_PATH "src")

set(FRAMEWORK_FILES
        ${SOURCE_PATH}/ArchTypes.cpp
        ${SOURCE_PATH}/Collector.cpp
        ${SOURCE_PATH}/Common.cpp
        ${SOURCE_PATH}/Component.cpp
        ${SOURCE_PATH}/Device.cpp
        ${SOURCE_PATH}/Connector.cpp
        ${SOURCE_PATH}/ContentItem.cpp
        ${SOURCE_PATH}/FileItem.cpp
        ${SOURCE_PATH}/ParameterItem.cpp
        ${SOURCE_PATH}/ExecutorItem.cpp
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
        ${SOURCE_PATH}/NodeObject.cpp
        ${SOURCE_PATH}/NodeManager.cpp
        ${SOURCE_PATH}/Scheduler.cpp
        ${SOURCE_PATH}/SchedulerItem.cpp
        ${SOURCE_PATH}/Util.cpp
        ${SOURCE_PATH}/StopWatch.cpp
        ${SOURCE_PATH}/UnixSocket.cpp
        ${SOURCE_PATH}/AddressHelper.cpp
        ${SOURCE_PATH}/Job.cpp
        ${SOURCE_PATH}/MessageItem.cpp
        ${SOURCE_PATH}/SchedulerItem.cpp
        ${SOURCE_PATH}/JsonType.cpp
        ${SOURCE_PATH}/JsonItem.cpp
        ${SOURCE_PATH}/Jobs.cpp
        #${SOURCE_PATH}/JobInfo.cpp
        ${SOURCE_PATH}/MessageHeader.cpp
        ${SOURCE_PATH}/Block.cpp
        ${SOURCE_PATH}/ComponentTypes.cpp
        ${SOURCE_PATH}/InterfaceTypes.cpp
        ${SOURCE_PATH}/DeviceList.cpp
        ${SOURCE_PATH}/MessageData.cpp
        #${SOURCE_PATH}/NodeTracker.cpp
        ${SOURCE_PATH}/NodeState.cpp
        ${SOURCE_PATH}/History.cpp
        ${SOURCE_PATH}/FileInfo.cpp
        ${SOURCE_PATH}/CollectorManager.cpp
        ${SOURCE_PATH}/ComponentObject.cpp
        ${SOURCE_PATH}/ComponentManager.cpp
        ${SOURCE_PATH}/CollectorObject.cpp
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

if (APPLE)
    set(JSON-C_INCLUDE_DIRS ${CMAKE_SYSROOT}/usr/local/Cellar/json-c/0.12.1/include/json-c)
    set(OPENSSL_INCLUDE_DIR ${CMAKE_SYSROOT}/usr/local/Cellar/openssl/1.0.2n/include/)
else()
    set(JSON-C_INCLUDE_DIRS ${CMAKE_SYSROOT}/usr/include/json-c)
    set(OPENSSL_INCLUDE_DIR ${CMAKE_SYSROOT}/usr/include)
endif()

set(JSON-C_LIBRARIES json-c)
set(OPENSSL_CRYPTO_LIBRARY crypto)

add_definitions(-DMULTICAST_ENABLED)
add_definitions(-DDISABLE_RECOVERY)
add_definitions(-DDISABLE_BACKUP)
add_definitions(-DDISABLE_LOGFILEINFO)
add_definitions(-DMULTICAST_ENABLED)

include_directories(${SOURCE_PATH})
include_directories(${JSON-C_INCLUDE_DIRS})

add_library(bankor-framework ${FRAMEWORK_FILES})
target_include_directories(bankor-framework PUBLIC ${OPENSSL_INCLUDE_DIR})

if (NOT DEFINED CONSOLE)

    find_package(wxWidgets)

    add_executable(bankor-wx ${WX_FILES})
    target_compile_definitions(bankor-wx PUBLIC ${wxWidgets_DEFINITIONS})
    target_include_directories(bankor-wx PUBLIC ${wxWidgets_INCLUDE_DIRS})
    target_compile_options(bankor-wx PUBLIC ${wxWidgets_CXX_FLAGS})
    target_link_libraries(bankor-wx bankor-framework
            ${JSON-C_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARY} ${wxWidgets_LIBRARIES} )
endif()

add_executable(bankor-console ${CONSOLE_FILES})
target_link_libraries(bankor-console bankor-framework
        ${JSON-C_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARY} pthread)



