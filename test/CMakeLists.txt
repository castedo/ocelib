cmake_minimum_required(VERSION 2.8.1)

add_executable(ocelib-test test.cpp jios_test.cpp)
target_link_libraries(ocelib-test ocelib ${Boost_LIBRARIES})

add_test(NAME ocelib-test COMMAND ocelib-test -l message)

