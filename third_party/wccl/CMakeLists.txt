PROJECT(WCCL)
cmake_minimum_required(VERSION 2.8.0)

set(wccl_ver_major "0")
set(wccl_ver_minor "3")
set(wccl_ver_patch "2")

set(LIBWCCL_VERSION "${wccl_ver_major}.${wccl_ver_minor}.${wccl_ver_patch}")

# use some of our own Find* scripts
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeScripts)

IF(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
		"Choose the type of build, options are: None Debug Release
		RelWithDebInfo MinSizeRel."
		FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

set(WCCL_BUILD_SWIG True CACHE BOOL "Build SWIG Python wrappers" FORCE)

if(UNIX)
	set(CFLAG_ANSI "-ansi")
else()
	set(CFLAG_ANSI "")
endif(UNIX)

if(CMAKE_COMPILER_IS_GNUCXX)
	# Set our own default flags at first run.
	if(NOT CONFIGURED)
		if(ENABLE_STRICT_COMPILATION)
			set(STRICT_FLAGS "-Werror -Wno-unused-parameter")
		else(ENABLE_STRICT_COMPILATION)
			set(STRICT_FLAGS "")
		endif(ENABLE_STRICT_COMPILATION)

		set(CMAKE_C_FLAGS "-W -Wall ${CFLAG_ANSI} $ENV{CFLAGS}" 
			CACHE STRING "Flags used by the C compiler during normal builds."
			FORCE)
		set(CMAKE_C_FLAGS_DEBUG "-O0 -DDEBUG -ggdb3 -W -Wall ${CFLAG_ANSI} $ENV{CFLAGS}"
			CACHE STRING "Flags used by the C compiler during debug builds."
			FORCE)

		set(CMAKE_CXX_FLAGS "-W -Wall ${CFLAG_ANSI} ${STRICT_FLAGS} $ENV{CXXFLAGS}"
			CACHE STRING "Flags used by the CXX compiler during normal builds."
			FORCE)
		set(CMAKE_CXX_FLAGS_DEBUG "-O0 -DDEBUG -ggdb3 -W -Wall ${CFLAG_ANSI} ${STRICT_FLAGS} $ENV{CXXFLAGS}"
			CACHE STRING "Flags used by the CXX compiler during debug builds."
			FORCE)
                set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -fno-omit-frame-pointer" 
                        CACHE STRING "Flags used by the CXX compiler during relwithdebiginfo builds" FORCE)
	endif(NOT CONFIGURED)
endif(CMAKE_COMPILER_IS_GNUCXX)

set(LIBS "")
include_directories(${WCCL_SOURCE_DIR})

find_package(Boost 1.41 REQUIRED COMPONENTS program_options system filesystem regex)

if(MSVC OR BORLAND)
# Use the auto-linking feature, don't try to add libraries yourself:
	set(Boost_LIBRARIES "")
# Note the default is to link static libs and it is left like that here.
# You can make it link to DLL by defining BOOST_#module#_DYN_LINK
# in C++ source file or somewhere in CMake file, e.g. 
# add_definitions( -DBOOST_FILESYSTEM_DYN_LINK )
endif(MSVC OR BORLAND)

add_subdirectory(libwccl)
add_subdirectory(wccl-apps)
if(UNIX)
	add_subdirectory(tests)
endif(UNIX)
add_subdirectory(libmwereader)

if(WCCL_BUILD_SWIG)
	FIND_PACKAGE(SWIG)
	if(SWIG_FOUND)
		message(STATUS "SWIG found, Python wrappers will be built")
		add_subdirectory(swig)
	else()
		message(STATUS "WARNING: SWIG not found, Python wrappers will not be built")
	endif(SWIG_FOUND)
else()
	message(STATUS "Not building SWIG Python wrappers")
endif(WCCL_BUILD_SWIG)

