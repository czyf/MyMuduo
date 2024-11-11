cmake_minimum_required(VERSION 3.26.0)

project(mymuduo)

# 设置动态库输出路径
set(CMAKE_LIBRARY_PATH ${PROJECT_SOURCE_DIR}/lib)
# 设置调试与标准版本
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=17")

aux_source_directory(. SRC_LIST)
add_library(mymuduo SHARED ${SRC_LIST})