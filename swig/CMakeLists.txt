cmake_minimum_required(VERSION 2.8)

FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})
SET(CMAKE_SWIG_FLAGS "")

add_subdirectory(r)
