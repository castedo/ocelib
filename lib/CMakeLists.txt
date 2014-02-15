cmake_minimum_required(VERSION 2.8.1)

add_library(ocelib
    jios/json_oj.cpp
)

target_link_libraries(ocelib
    ocel-proto-lib
    ${Boost_LIBRARIES}
    # json_linux-gcc_libmt
)

