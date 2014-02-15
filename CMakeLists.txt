cmake_minimum_required(VERSION 2.8.1)

project(ocelib)

# Uncomment for debugging build:
# set(CMAKE_VERBOSE_MAKEFILE ON)
# add_definitions(-H)

add_definitions(-std=c++0x)

include_directories(.)

### google protocol buffers

find_package(Protobuf REQUIRED)

include_directories(${PROTOBUF_INCLUDE_DIRS})

# precompiled headers and google protobuf generated headers files:
include_directories(${CMAKE_BINARY_DIR})

# project parts

add_subdirectory(proto)
add_subdirectory(lib)

