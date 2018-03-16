INCLUDE(CMakeForceCompiler)
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)

set(TOOLCHAIN_DIR  /opt/toolchains)
set(TOOLCHAIN_TYPE x86_64-siemens-linux-gnu)
set(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}/${TOOLCHAIN_TYPE}/bin/${TOOLCHAIN_TYPE}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}/${TOOLCHAIN_TYPE}/bin/${TOOLCHAIN_TYPE}-gcc)
set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_LINKER> <FLAGS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_LINKER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
set(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_DIR}/${TOOLCHAIN_TYPE}/)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)


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