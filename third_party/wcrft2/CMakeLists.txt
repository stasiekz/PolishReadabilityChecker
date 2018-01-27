PROJECT(CRFTagger)

cmake_minimum_required(VERSION 2.8.0)

# use some of our own Find* scripts
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeScripts)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
		"Choose the type of build, options are: None Debug Release
		RelWithDebInfo MinSizeRel."
		FORCE)
endif(NOT CMAKE_BUILD_TYPE)

set(WCRFT_BUILD_SWIG True CACHE BOOL "Build SWIG Python wrappers" FORCE)

# compiler flags
if(CMAKE_COMPILER_IS_GNUCXX)
	# Set our own default flags at first run.
	if(NOT CONFIGURED)
		if(ENABLE_STRICT_COMPILATION)
			set(STRICT_FLAGS "-Werror -Wno-unused-parameter")
		else(ENABLE_STRICT_COMPILATION)
			set(STRICT_FLAGS "")
		endif(ENABLE_STRICT_COMPILATION)

		set(CMAKE_C_FLAGS "-W -Wall -ansi $ENV{CFLAGS}" 
			CACHE STRING "Flags used by the C compiler during normal builds."
			FORCE)
		set(CMAKE_C_FLAGS_DEBUG "-O0 -DDEBUG -ggdb3 -W -Wall -ansi $ENV{CFLAGS}"
			CACHE STRING "Flags used by the C compiler during debug builds."
			FORCE)

		set(CMAKE_CXX_FLAGS "-W -Wall -ansi ${STRICT_FLAGS} $ENV{CXXFLAGS}"
			CACHE STRING "Flags used by the CXX compiler during normal builds."
			FORCE)
		set(CMAKE_CXX_FLAGS_DEBUG "-O0 -DDEBUG -ggdb3 -W -Wall -ansi ${STRICT_FLAGS} $ENV{CXXFLAGS}"
			CACHE STRING "Flags used by the CXX compiler during debug builds."
			FORCE)
                set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -fno-omit-frame-pointer" 
                        CACHE STRING "Flags used by the CXX compiler during relwithdebiginfo builds" FORCE)
	endif(NOT CONFIGURED)
endif(CMAKE_COMPILER_IS_GNUCXX)

set(LIBS "")
include_directories(${CRFTagger_SOURCE_DIR})

#required packages
find_package(Boost 1.41 REQUIRED COMPONENTS program_options filesystem)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})
set(LIBS ${LIBS} ${Boost_LIBRARIES})

find_package(CRFPP 0.58 REQUIRED)
set(LIBS ${LIBS} ${CRFPP_LIBRARY})
include_directories(${CRFPP_INCLUDE_DIR})

find_package(PwrUtils REQUIRED)
set(LIBS ${LIBS} ${PwrUtils_LIBRARY})
include_directories(${PwrUtils_INCLUDE_DIR})

find_package(Corpus2 1.0.9 REQUIRED)
set(LIBS ${LIBS} ${Corpus2_LIBRARY})
include_directories(${Corpus2_INCLUDE_DIR})

find_package(Maca 1.0.7 REQUIRED)
set(LIBS ${LIBS} ${Maca_LIBRARY})
include_directories(${Maca_INCLUDE_DIR})

find_package(WCCL 0.3.2 REQUIRED)
set(LIBS ${LIBS} ${WCCL_LIBRARY})
include_directories(${WCCL_INCLUDE_DIR})

add_subdirectory(libwcrft)
add_subdirectory(wcrft-app)

if(WCRFT_BUILD_SWIG)
	FIND_PACKAGE(SWIG)
	if(SWIG_FOUND)
		message(STATUS "SWIG found, Python wrappers will be built")
		add_subdirectory(swig)
	else()
		message(STATUS "WARNING: SWIG not found, Python wrappers will not be built")
	endif(SWIG_FOUND)
else()
	message(STATUS "Not building SWIG Python wrappers")
endif(WCRFT_BUILD_SWIG)

#building documentation
option(BUILD-DOCS "Building project documentation" OFF)
if(BUILD-DOCS)
	add_subdirectory(doc)
endif()

set(BUILD_WCRFT_READER True CACHE BOOL "Build WCRFT-as-reader library (plugin for Corpus2 that allows for easier integration with Corpus2-based tools)")
if(BUILD_WCRFT_READER)
	add_subdirectory(libwcrftreader)
endif()

message(STATUS "Use cmake wizard mode: -i; to manage build configuration.")
