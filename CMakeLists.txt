cmake_minimum_required(VERSION 3.9)

project(Dicon)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_VERBOSE_MAKEFILE 1)

set(SOURCE_PATH "src")

set(UI_PATH "${SOURCE_PATH}/ui")

set(EXTERNAL_PATH "${SOURCE_PATH}/external")

set(GENERATED_PATH "${SOURCE_PATH}/gen")

set(TEST_PATH "${SOURCE_PATH}/test")

set(FRAMEWORK_FILES
        ${EXTERNAL_PATH}/miniz/miniz.cpp
        ${SOURCE_PATH}/ArchType.cpp
        ${SOURCE_PATH}/Address.cpp
        ${SOURCE_PATH}/Application.cpp
        ${SOURCE_PATH}/Collector.cpp
        ${SOURCE_PATH}/CollectorHost.cpp
        ${SOURCE_PATH}/CollectorManager.cpp
        ${SOURCE_PATH}/CollectorObject.cpp
        ${SOURCE_PATH}/CollectorState.cpp
        ${SOURCE_PATH}/CollectorUnit.cpp
        ${SOURCE_PATH}/Common.cpp
        ${SOURCE_PATH}/CommUnit.cpp
        ${SOURCE_PATH}/Component.cpp
        ${SOURCE_PATH}/ComponentFactory.cpp
        ${SOURCE_PATH}/ComponentManager.cpp
        ${SOURCE_PATH}/ComponentObject.cpp
        ${SOURCE_PATH}/ComponentUnit.cpp
        ${SOURCE_PATH}/ComponentUnitFactory.cpp
        ${SOURCE_PATH}/ContentItem.cpp
        ${SOURCE_PATH}/Device.cpp
        ${SOURCE_PATH}/DeviceList.cpp
        ${SOURCE_PATH}/Distributor.cpp
        ${SOURCE_PATH}/DistributorHost.cpp
        ${SOURCE_PATH}/DistributorObject.cpp
        ${SOURCE_PATH}/FileItem.cpp
        ${SOURCE_PATH}/HostUnit.cpp
        ${SOURCE_PATH}/Interface.cpp
        ${SOURCE_PATH}/InterfaceCallback.cpp
        ${SOURCE_PATH}/InterfaceFactory.cpp
        ${SOURCE_PATH}/InterfaceType.cpp
        ${SOURCE_PATH}/JobItem.cpp
        ${SOURCE_PATH}/JobStatus.cpp
        ${SOURCE_PATH}/JsonType.cpp
        ${SOURCE_PATH}/Message.cpp
        ${SOURCE_PATH}/MessageBase.cpp
        ${SOURCE_PATH}/MessageBlockHeader.cpp
        ${SOURCE_PATH}/MessageData.cpp
        ${SOURCE_PATH}/MessageHeader.cpp
        ${SOURCE_PATH}/MessageItem.cpp
        ${SOURCE_PATH}/MessageItem.cpp
        ${SOURCE_PATH}/MessageType.cpp
        ${SOURCE_PATH}/Net.cpp
        ${SOURCE_PATH}/NetUtil.cpp
        ${SOURCE_PATH}/Node.cpp
        ${SOURCE_PATH}/NodeHost.cpp
        ${SOURCE_PATH}/NodeManager.cpp
        ${SOURCE_PATH}/NodeObject.cpp
        ${SOURCE_PATH}/NodeObject.cpp
        ${SOURCE_PATH}/NodeState.cpp
        ${SOURCE_PATH}/NodeUnit.cpp
        ${SOURCE_PATH}/ParameterItem.cpp
        ${SOURCE_PATH}/ProcessFile.cpp
        ${SOURCE_PATH}/ProcessItem.cpp
        ${SOURCE_PATH}/ProcessState.cpp
        ${SOURCE_PATH}/Scheduler.cpp
        ${SOURCE_PATH}/SchedulerItem.cpp
        ${SOURCE_PATH}/SchedulerItem.cpp
        ${SOURCE_PATH}/StopWatch.cpp
        ${SOURCE_PATH}/UnixSocket.cpp
        ${SOURCE_PATH}/Util.cpp
        )

set(CONSOLE_FILES
        ${UI_PATH}/console/ConsoleApp.cpp
        ${UI_PATH}/console/ConsoleDistributor.cpp
        ${UI_PATH}/console/ConsoleCollector.cpp
        ${UI_PATH}/console/ConsoleNode.cpp
        ${SOURCE_PATH}/Entry.cpp
        )

set(WEB_FILES
        ${EXTERNAL_PATH}/civetweb/civetweb.c
        ${UI_PATH}/web/WebApp.cpp
        ${UI_PATH}/web/WebOption.cpp
        ${UI_PATH}/web/WebDistributor.cpp
        ${UI_PATH}/web/WebCollector.cpp
        ${UI_PATH}/web/WebNode.cpp
        ${GENERATED_PATH}/WebPage.cpp
        ${SOURCE_PATH}/Entry.cpp
        )

set(GENERATOR_FILES
        ${EXTERNAL_PATH}/miniz/miniz.cpp
        ${SOURCE_PATH}/Util.cpp
        ${SOURCE_PATH}/Generator.cpp
        )

set(TEST_FILES
        ${TEST_PATH}/TestApp.cpp
        ${TEST_PATH}/TestPing.cpp
        ${TEST_PATH}/TestComponentList.cpp
        ${TEST_PATH}/TestProcess.cpp
        ${TEST_PATH}/TestFileInfo.cpp
        ${TEST_PATH}/TestFileBinary.cpp
        ${TEST_PATH}/TestJobName.cpp
        ${TEST_PATH}/TestLoadJob.cpp
        ${TEST_PATH}/TestWakeUp.cpp
        ${TEST_PATH}/TestPipeControl.cpp
        ${SOURCE_PATH}/Entry.cpp
        )

if (APPLE)
    set(JSON-C_INCLUDE_DIRS ${CMAKE_SYSROOT}/usr/local/opt/json-c/include/json-c)
    set(LIBRARY_DIR ${CMAKE_SYSROOT}/usr/local/lib)
else()
    set(JSON-C_INCLUDE_DIRS ${CMAKE_SYSROOT}/usr/include/json-c)
    set(LIBRARY_DIR ${CMAKE_SYSROOT}/usr/lib)
endif()

set(JSON-C_LIBRARIES json-c)

add_definitions(-DDISABLE_RECOVERY)
add_definitions(-DDISABLE_BACKUP)
add_definitions(-DDISABLE_LOGFILEINFO)
add_definitions(-DMULTICAST_ENABLED)

include_directories(${SOURCE_PATH} ${UI_PATH} ${EXTERNAL_PATH})
include_directories(${JSON-C_INCLUDE_DIRS})
link_directories(${LIBRARY_DIR})

add_library(dicon-framework ${FRAMEWORK_FILES})

add_executable(dicon-generator ${GENERATOR_FILES})

add_executable(dicon-console ${CONSOLE_FILES})
target_compile_definitions(dicon-console PUBLIC -DCONSOLE)
target_link_libraries(dicon-console dicon-framework ${JSON-C_LIBRARIES} pthread)

add_executable(dicon-web ${WEB_FILES})
target_compile_definitions(dicon-web PUBLIC -DWEB -DNO_SSL)
target_link_libraries(dicon-web dicon-framework ${JSON-C_LIBRARIES} pthread)

add_executable(dicon-test ${TEST_FILES})
target_compile_definitions(dicon-test PUBLIC -DTEST)
target_link_libraries(dicon-test dicon-framework ${JSON-C_LIBRARIES} pthread)

add_custom_command(
        OUTPUT  ${CMAKE_CURRENT_SOURCE_DIR}/${GENERATED_PATH}/WebPage.cpp
        COMMAND dicon-generator
        DEPENDS dicon-generator
)
