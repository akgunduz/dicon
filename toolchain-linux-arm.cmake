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

set(TARGET_SUFFIX lnx)