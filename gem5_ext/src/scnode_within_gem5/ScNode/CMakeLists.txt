
#target_include_directories(scnode PUBLIC $ENV{ICSC_HOME}/include)

#add_library(systemc INTERFACE)
#find_library(systemc systemc HINTS $ENV{ICSC_HOME}/lib $ENV{ICSC_HOME}/lib)
#target_link_libraries(scnode PUBLIC systemc)
cmake_minimum_required(VERSION 3.10)

project(scnode VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIBRARY_OUTPUT_NAME scnode)

# 设置 SystemC 的头文件和库路径（请根据你的安装路径修改）
set(SYSTEMC_INCLUDE_DIR $ENV{ICSC_HOME}/include)
set(SYSTEMC_LIB_DIR $ENV{ICSC_HOME}/lib)

include_directories(${SYSTEMC_INCLUDE_DIR})

set(SOURCES
    ScNodeTarget.cpp
    ScNodeWrapper.cpp
    DebugManager.cpp
)

add_library(${LIBRARY_OUTPUT_NAME} SHARED ${SOURCES})

add_compile_definitions(MTContext)

target_compile_options(${LIBRARY_OUTPUT_NAME}
    PRIVATE -fvisibility=hidden
)

# 设置 PIC（Position Independent Code）以支持动态加载
set_target_properties(${LIBRARY_OUTPUT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

# 链接 SystemC 库和其他依赖
target_link_libraries(${LIBRARY_OUTPUT_NAME}
    -L${SYSTEMC_LIB_DIR}
    -lsystemc
    -lm
    -ldl
)

# 可选：指定生成的共享库输出路径
set_target_properties(${LIBRARY_OUTPUT_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    LINK_DIRECTOREIS ${SYSTEMC_LIB_DIR}
    INSTALL_RPATH ${SYSTEMC_LIB_DIR}
    BUILD_RPATH "${SYSTEMC_LIB_DIR}"
    RPATH "${SYSTEMC_LIB_DIR}"
)

install(TARGETS ${LIBRARY_OUTPUT_NAME}
    LIBRARY DESTINATION lib
)
