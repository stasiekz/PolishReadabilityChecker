PROJECT(Tokenizer)
cmake_minimum_required(VERSION 2.6.0)

# use some of our own Find* scripts
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeScripts)

IF(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
		"Choose the type of build, options are: None Debug Release
		RelWithDebInfo MinSizeRel."
		FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)


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

find_package(Boost 1.41 REQUIRED COMPONENTS program_options filesystem system regex)

add_subdirectory(libtoki)
if(UNIX)
add_subdirectory(tests)
endif(UNIX)
add_subdirectory(toki-app)
