cmake_minimum_required(VERSION 3.10)
project(CS6120 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_library(CFGlib STATIC
    lib/cfg/cfg.cpp
)
target_include_directories(
    CFGlib PUBLIC ${PROJECT_SOURCE_DIR}/include
)

add_library(LVNlib STATIC
    lib/lvn/lvn.cpp
)
target_include_directories(
    LVNlib PUBLIC ${PROJECT_SOURCE_DIR}/include
)


add_executable(cfg src/l2_cfg.cpp)
target_link_libraries(cfg CFGlib)

add_executable(dce src/l3/l3_dce.cpp)
target_link_libraries(dce CFGlib)

add_executable(lvn src/l3/l3_lvn.cpp)
target_link_libraries(lvn CFGlib LVNlib)
