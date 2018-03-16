INCLUDE(CMakeForceCompiler)
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)

set(TOOLCHAIN_DIR  E:/Toolchains)
set(TOOLCHAIN_TYPE x86_64-siemens-linux-gnu)
set(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}/${TOOLCHAIN_TYPE}/bin/${TOOLCHAIN_TYPE}-gcc.exe)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}/${TOOLCHAIN_TYPE}/bin/${TOOLCHAIN_TYPE}-gcc.exe)
set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_LINKER> <FLAGS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_LINKER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
set(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_DIR}/${TOOLCHAIN_TYPE}/)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)


add_definitions(-DUNICODE -DCYGWIN)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/lib/wx/include/gtk2-unicode-3.0 -I/usr/include/wx-3.0
        -D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXGTK__")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /bin/cygjson-c-2.dll /bin/cygcrypto-1.0.0.dll
        -lwx_gtk2u_xrc-3.0 -lwx_gtk2u_webview-3.0 -lwx_gtk2u_html-3.0 -lwx_gtk2u_qa-3.0
        -lwx_gtk2u_adv-3.0 -lwx_gtk2u_core-3.0 -lwx_baseu_xml-3.0 -lwx_baseu_net-3.0 -lwx_baseu-3.0")

add_executable(bankor-cyg ${FILES})