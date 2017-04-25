cmake_minimum_required(VERSION 2.8.1)

add_library(ocelib
    utility.cpp
    parse.cpp
)

target_link_libraries(ocelib
    # deprecated ocel-proto-lib
    ${Boost_LIBRARIES}
)

