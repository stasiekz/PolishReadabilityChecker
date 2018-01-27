project(corpus2mwe)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

set(corpus2_mwe_ver_major "0")
set(corpus2_mwe_ver_minor "1")
set(corpus2_mwe_ver_patch "0")

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeScripts)
set(CMAKE_BUILD_TYPE Debug)

set(LIBS "")
include_directories(${corpus2mwe_SOURCE_DIR})

add_subdirectory(cclmwe)
find_package(SWIG)
if(SWIG_FOUND)
	message(STATUS "SWIG found - building Python wrappers")
	add_subdirectory(swig)
else()
	message(STATUS "WARNING: SWIG not found - Python wrappers will not be built")
endif(SWIG_FOUND)
