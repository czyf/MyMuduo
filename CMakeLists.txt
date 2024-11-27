cmake_minimum_required(VERSION 3.22.0)

project(mymuduo)

# 设置动态库输出路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
# 设置调试信息与语言标准
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++17 -fPIC")

# set(CMAKE_VERBOSE_MAKEFILE ON) 

aux_source_directory(. SRC_LIST)
add_library(mymuduo SHARED ${SRC_LIST})