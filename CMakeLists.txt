cmake_minimum_required(VERSION 3.10)
project(CS6120 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_library(CFGlib STATIC
    lib/cfg/cfg.cpp
)
target_include_directories(
    CFGlib PUBLIC ${PROJECT_SOURCE_DIR}/include
)

add_executable(l2_cfg src/l2_cfg.cpp)
target_link_libraries(l2_cfg CFGlib)
add_executable(l3_dce src/l3/l3_dce.cpp)
target_link_libraries(l3_dce CFGlib)
