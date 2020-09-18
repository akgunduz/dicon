cmake_minimum_required(VERSION 3.9)

project(Dicon)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_VERBOSE_MAKEFILE 1)

set(SOURCE_PATH "src")

set(UI_PATH "${SOURCE_PATH}/ui")

set(EXTERNAL_PATH "${SOURCE_PATH}/external")

set(GENERATED_PATH "${SOURCE_PATH}/gen")

set(TEST_PATH "${SOURCE_PATH}/test")

set(CIVETWEB_FILES
        ${EXTERNAL_PATH}/civetweb/civetweb.c
        )

set(MINIZ_FILES
        ${EXTERNAL_PATH}/miniz/miniz.c
        )

set(FRAMEWORK_FILES
        ${SOURCE_PATH}/Address.cpp
        ${SOURCE_PATH}/Application.cpp
        ${SOURCE_PATH}/BaseUnit.cpp
        ${SOURCE_PATH}/Collector.cpp
        ${SOURCE_PATH}/CollectorHost.cpp
        ${SOURCE_PATH}/CollectorManager.cpp
        ${SOURCE_PATH}/CollectorObject.cpp
        ${SOURCE_PATH}/CollectorUnit.cpp
        ${SOURCE_PATH}/CommInterface.cpp
        ${SOURCE_PATH}/CommInterfaceCB.cpp
        ${SOURCE_PATH}/CommInterfaceFactory.cpp
        ${SOURCE_PATH}/CommPipe.cpp
        ${SOURCE_PATH}/CommTCP.cpp
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
        ${SOURCE_PATH}/JobItem.cpp
        ${SOURCE_PATH}/Message.cpp
        ${SOURCE_PATH}/MessageBase.cpp
        ${SOURCE_PATH}/MessageBlock.cpp
        ${SOURCE_PATH}/MessageData.cpp
        ${SOURCE_PATH}/MessageHeader.cpp
        ${SOURCE_PATH}/MessageItem.cpp
        ${SOURCE_PATH}/MessageItem.cpp
        ${SOURCE_PATH}/MessageType.cpp
        ${SOURCE_PATH}/NetUtil.cpp
        ${SOURCE_PATH}/Node.cpp
        ${SOURCE_PATH}/NodeHost.cpp
        ${SOURCE_PATH}/NodeManager.cpp
        ${SOURCE_PATH}/NodeObject.cpp
        ${SOURCE_PATH}/NodeObject.cpp
        ${SOURCE_PATH}/NodeUnit.cpp
        ${SOURCE_PATH}/ParameterItem.cpp
        ${SOURCE_PATH}/ProcessFile.cpp
        ${SOURCE_PATH}/ProcessItem.cpp
        ${SOURCE_PATH}/Scheduler.cpp
        ${SOURCE_PATH}/SchedulerItem.cpp
        ${SOURCE_PATH}/SchedulerItem.cpp
        ${SOURCE_PATH}/StopWatch.cpp
        ${SOURCE_PATH}/Util.cpp
        ${SOURCE_PATH}/UvUtil.cpp
        )

set(CONSOLE_FILES
        ${UI_PATH}/console/ConsoleApp.cpp
        ${UI_PATH}/console/ConsoleDistributor.cpp
        ${UI_PATH}/console/ConsoleCollector.cpp
        ${UI_PATH}/console/ConsoleNode.cpp
        ${SOURCE_PATH}/Entry.cpp
        )

set(WEB_FILES
        ${UI_PATH}/web/WebApp.cpp
        ${UI_PATH}/web/WebOption.cpp
        ${UI_PATH}/web/WebDistributor.cpp
        ${UI_PATH}/web/WebCollector.cpp
        ${UI_PATH}/web/WebNode.cpp
        ${GENERATED_PATH}/WebPage.cpp
        ${SOURCE_PATH}/Entry.cpp
        )

set(GENERATOR_FILES
        ${SOURCE_PATH}/Util.cpp
        ${SOURCE_PATH}/Generator.cpp
        )

set(TEST_FILES
        ${TEST_PATH}/TestApp.cpp
        ${TEST_PATH}/TestSendJobName.cpp
        ${TEST_PATH}/TestSendFileBinary.cpp
        ${TEST_PATH}/TestSendFileInfo.cpp
        ${TEST_PATH}/TestSendProcessID.cpp
        ${TEST_PATH}/TestSendProcessInfo.cpp
        ${TEST_PATH}/TestSendProcessFileBinary.cpp
        ${TEST_PATH}/TestSendProcessFilesBinary.cpp
        ${TEST_PATH}/TestSendProcess.cpp
        ${TEST_PATH}/TestSendComponentList.cpp
        ${TEST_PATH}/TestSendWakeUp.cpp
        ${TEST_PATH}/TestSendID.cpp

        ${TEST_PATH}/TestPing.cpp
        ${TEST_PATH}/TestLoadJob.cpp
        ${TEST_PATH}/TestCRC.cpp

        ${TEST_PATH}/TestFileExecution.cpp
        ${TEST_PATH}/TestProcessExecution.cpp

        ${SOURCE_PATH}/Entry.cpp
        )

include_directories(${SOURCE_PATH} ${UI_PATH} ${EXTERNAL_PATH})
include_directories(${EXTERNAL_PATH}/uv/include)

set(LIBRARIES miniz uv_a)

if(WIN32)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_compile_definitions(INCL_EXTRA_HTON_FUNCTIONS)
    add_compile_definitions(NOMINMAX)
    list(APPEND LIBRARIES ws2_32 Iphlpapi)
else()
    list(APPEND LIBRARIES pthread)
endif()

##################################################
add_library(civetweb STATIC ${CIVETWEB_FILES})
target_link_libraries(civetweb dl)
##################################################
add_library(miniz STATIC ${MINIZ_FILES})
##################################################
set (LIBUV_BUILD_TESTS OFF)
set (LIBUV_BUILD_BENCH OFF)
add_subdirectory(${EXTERNAL_PATH}/uv EXCLUDE_FROM_ALL)
##################################################
add_library(dicon-framework ${FRAMEWORK_FILES})
target_link_libraries(dicon-framework PRIVATE)
##################################################

add_executable(dicon-generator ${GENERATOR_FILES})

##################################################

add_executable(dicon-console ${CONSOLE_FILES})
target_compile_definitions(dicon-console PUBLIC -DCONSOLE)
target_link_libraries(dicon-console dicon-framework ${LIBRARIES})

##################################################

add_custom_command(
        OUTPUT  ${CMAKE_CURRENT_SOURCE_DIR}/${GENERATED_PATH}/WebPage.cpp
        COMMAND dicon-generator
        DEPENDS dicon-generator
)

add_executable(dicon-web ${WEB_FILES})
target_compile_definitions(dicon-web PUBLIC -DWEB -DNO_SSL)
target_link_libraries(dicon-web dicon-framework civetweb ${LIBRARIES})

##################################################

add_executable(dicon-test ${TEST_FILES})
target_compile_definitions(dicon-test PUBLIC -DTEST)
target_link_libraries(dicon-test dicon-framework ${LIBRARIES})

##################################################
