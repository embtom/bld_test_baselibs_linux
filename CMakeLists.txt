cmake_minimum_required(VERSION 3.7)
project (bld_test_cmake_baselibs_emb)

SET(CMAKE_VERBOSE_MAKEFILE OFF)   
SET(CMAKE_DEBUG_POSTFIX _g)


add_subdirectory(io)
add_subdirectory(test)
