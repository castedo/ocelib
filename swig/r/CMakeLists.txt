cmake_minimum_required(VERSION 2.8)

include_directories("/usr/lib/R/include/")
include_directories("/usr/lib64/R/include/")

set_source_files_properties(test.i PROPERTIES CPLUSPLUS ON)
swig_add_module(test_swig_R R test.cpp test.i)
set_target_properties(test_swig_R PROPERTIES PREFIX "")

add_test(NAME test_swig_R
         COMMAND Rscript ${CMAKE_CURRENT_SOURCE_DIR}/test.R)

