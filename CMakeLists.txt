cmake_minimum_required(VERSION 3.9)

project(Dicon)

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
        ${SOURCE_PATH}/ProcessItem.cpp
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
        ${SOURCE_PATH}/Timer.cpp
        ${SOURCE_PATH}/UnixSocket.cpp
        ${SOURCE_PATH}/AddressHelper.cpp
        ${SOURCE_PATH}/Job.cpp
        ${SOURCE_PATH}/MessageItem.cpp
        ${SOURCE_PATH}/SchedulerItem.cpp
        ${SOURCE_PATH}/JsonType.cpp
        ${SOURCE_PATH}/JsonItem.cpp
        ${SOURCE_PATH}/MessageHeader.cpp
        ${SOURCE_PATH}/Block.cpp
        #${SOURCE_PATH}/ComponentTypes.cpp
        ${SOURCE_PATH}/InterfaceTypes.cpp
        ${SOURCE_PATH}/DeviceList.cpp
        ${SOURCE_PATH}/MessageData.cpp
        #${SOURCE_PATH}/NodeTracker.cpp
        ${SOURCE_PATH}/NodeState.cpp
        ${SOURCE_PATH}/History.cpp
        ${SOURCE_PATH}/FileInfo.cpp
        ${SOURCE_PATH}/CollectorManager.cpp
        ${SOURCE_PATH}/ComponentObject.cpp
        ${SOURCE_PATH}/ComponentController.cpp
        ${SOURCE_PATH}/ComponentManager.cpp
        ${SOURCE_PATH}/CollectorObject.cpp
        ${SOURCE_PATH}/DistributorObject.cpp
        ${SOURCE_PATH}/ProcessInfo.cpp
        ${SOURCE_PATH}/JobOrdered.cpp
        ${SOURCE_PATH}/ComponentInfo.cpp
        ${SOURCE_PATH}/Application.cpp
        )

set(WX_FILES
        ${SOURCE_PATH}/ui/wx/Wx.cpp
        ${SOURCE_PATH}/ui/wx/WxComponent.cpp
        ${SOURCE_PATH}/ui/wx/WxDistributor.cpp
        ${SOURCE_PATH}/ui/wx/WxCollector.cpp
        ${SOURCE_PATH}/ui/wx/WxNode.cpp
        ${SOURCE_PATH}/ui/wx/WxMain.cpp
        ${SOURCE_PATH}/ui/wx/WxApp.cpp
        ${SOURCE_PATH}/Entry.cpp
        )

set(CONSOLE_FILES
        ${SOURCE_PATH}/ui/console/ConsoleEvent.cpp
        ${SOURCE_PATH}/ui/console/ConsoleApp.cpp
        ${SOURCE_PATH}/ui/console/ConsoleDistributor.cpp
        ${SOURCE_PATH}/ui/console/ConsoleCollector.cpp
        ${SOURCE_PATH}/ui/console/ConsoleNode.cpp
        ${SOURCE_PATH}/Entry.cpp
        )

set(WEB_FILES
        ${SOURCE_PATH}/ui/web/civetweb.c
        ${SOURCE_PATH}/ui/web/WebEvent.cpp
        ${SOURCE_PATH}/ui/web/WebApp.cpp
        ${SOURCE_PATH}/ui/web/WebDistributor.cpp
        ${SOURCE_PATH}/ui/web/WebCollector.cpp
        ${SOURCE_PATH}/ui/web/WebNode.cpp
        ${SOURCE_PATH}/Entry.cpp
        )

if (APPLE)
    set(JSON-C_INCLUDE_DIRS ${CMAKE_SYSROOT}/usr/local/opt/json-c/include/json-c)
    set(OPENSSL_INCLUDE_DIR ${CMAKE_SYSROOT}/usr/local/opt/openssl/include)
    set(LIBRARY_DIR ${CMAKE_SYSROOT}/usr/local/lib)
else()
    set(JSON-C_INCLUDE_DIRS ${CMAKE_SYSROOT}/usr/include/json-c)
    set(OPENSSL_INCLUDE_DIR ${CMAKE_SYSROOT}/usr/include)
    set(LIBRARY_DIR ${CMAKE_SYSROOT}/usr/lib)
endif()

set(JSON-C_LIBRARIES json-c)
set(OPENSSL_CRYPTO_LIBRARY crypto)

add_definitions(-DDISABLE_RECOVERY)
add_definitions(-DDISABLE_BACKUP)
add_definitions(-DDISABLE_LOGFILEINFO)
add_definitions(-DMULTICAST_ENABLED)

include_directories(${SOURCE_PATH} ${SOURCE_PATH}/ui)
include_directories(${JSON-C_INCLUDE_DIRS})
link_directories(${LIBRARY_DIR})

add_library(dicon-framework ${FRAMEWORK_FILES})
target_include_directories(dicon-framework PUBLIC ${OPENSSL_INCLUDE_DIR})

#add_executable(dicon-wx ${WX_FILES})
#find_package(wxWidgets)
#target_compile_definitions(dicon-wx PUBLIC ${wxWidgets_DEFINITIONS} -DWX_UI)
#target_include_directories(dicon-wx PUBLIC ${wxWidgets_INCLUDE_DIRS})
#target_compile_options(dicon-wx PUBLIC ${wxWidgets_CXX_FLAGS})
#target_link_libraries(dicon-wx dicon-framework
#        ${JSON-C_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARY} ${wxWidgets_LIBRARIES} )

#add_executable(dicon-console ${CONSOLE_FILES})
#target_compile_definitions(dicon-console PUBLIC -DCONSOLE_UI)
#target_link_libraries(dicon-console dicon-framework
#        ${JSON-C_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARY} pthread)

add_executable(dicon-web ${WEB_FILES})
target_compile_definitions(dicon-web PUBLIC -DWEB_UI -DUSE_WEBSOCKET -DNO_SSL)
#target_compile_options(dicon-web PUBLIC -Wall -Wextra -Wshadow -Wformat-security -Winit-self -Wmissing-prototypes -DLINUX -Iinclude  -DUSE_STACK_SIZE=102400 -O2 -DNDEBUG -fPIC)
target_link_libraries(dicon-web dicon-framework
        ${JSON-C_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARY} pthread dl rt)
