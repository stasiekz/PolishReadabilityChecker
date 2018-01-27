PROJECT(MorphAnalyser)
cmake_minimum_required(VERSION 2.8.0)

set(maca_ver_major "1")
set(maca_ver_minor "0")
set(maca_ver_patch "7")

set(BUILD_GPL_PLUGINS OFF CACHE BOOL "Include GPL plugins into installation")

# use some of our own Find* scripts
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeScripts)

IF(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
		"Choose the type of build, options are: None Debug Release
		RelWithDebInfo MinSizeRel."
		FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

set(MACA_BUILD_SWIG True CACHE BOOL "Build SWIG Python wrappers" FORCE)

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
include_directories(${MorphAnalyser_SOURCE_DIR})

FIND_PATH(LIBMACA_SRC_DATA_DIR
	test.ini
	${CMAKE_SOURCE_DIR}/data
)
MARK_AS_ADVANCED(LIBMACA_SRC_DATA_DIR)

find_package(Boost 1.41 REQUIRED COMPONENTS program_options filesystem system regex thread)

add_subdirectory(libmaca)
add_subdirectory(maca-analyse)
add_subdirectory(maca-reanalyse)
add_subdirectory(maca-convert)

if(UNIX)
add_subdirectory(tests)
endif()

if(MACA_BUILD_SWIG)
	FIND_PACKAGE(SWIG)
	if(SWIG_FOUND)
		message(STATUS "SWIG found, Python wrappers will be built")
		add_subdirectory(swig)
	else()
		message(STATUS "WARNING: SWIG not found, Python wrappers will not be built")
	endif(SWIG_FOUND)
else()
	message(STATUS "Not building SWIG Python wrappers")
endif(MACA_BUILD_SWIG)

if (UNIX)
	install(FILES tools/bash_completion_maca DESTINATION /etc/bash_completion.d)
endif(UNIX)

message(STATUS "*****************************************************")
if(BUILD_GPL_PLUGINS)
	message(STATUS "GPL components have been included into installtion:")
	if(SFST_COMPONENT_BUILD)
		message(STATUS "\tSFST")
	endif(SFST_COMPONENT_BUILD)
	if(GUESSER_COMPONENT_BUILD)
		message(STATUS "\tGuesser")
	endif(GUESSER_COMPONENT_BUILD)
else(BUILD_GPL_PLUGINS)
	message(STATUS "No GPL components have been included into installation.")
endif(BUILD_GPL_PLUGINS)

message(STATUS "Use cmake wizard mode: -i; to manage build configuration.")
message(STATUS "*****************************************************")
