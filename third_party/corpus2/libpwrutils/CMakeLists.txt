########## libpwrutils ###############

PROJECT(pwrutils)

set(pwrutils_ver_major "1")
set(pwrutils_ver_minor "0")
set(pwrutils_ver_patch "1")

set(LIBPWRUTILS_VERSION
	"${pwrutils_ver_major}.${pwrutils_ver_minor}.${pwrutils_ver_patch}")


configure_file(version.in include/libpwrutils/version.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include/)
add_definitions(-DHAVE_VERSION_H)
set(LIBS "")
if(UNIX)
    set(LIBS ${LIBS} dl)
endif(UNIX)

include_directories( ${CMAKE_SOURCE_DIR} )

find_package(ICU REQUIRED)
include_directories(${ICU_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIR})
link_directories(${ICU_LIBRARY_DIRS})
set(LIBS ${LIBS} icuuc icuio)

link_directories(${Boost_LIBRARY_DIRS})
set(LIBS ${LIBS} ${Boost_LIBRARIES})

SET(libpwrutils_STAT_SRC
	exception.cpp
	whitespace.cpp
	pathsearch.cpp
	plugin.cpp
	plural.cpp
	util.cpp
)

file(GLOB_RECURSE INCS "*.h")

if(WIN32)
# For DLLs on Windows (aka SHARED libraries) you have to explicitly
# specify the external API of the library. Nothing is exported
# by default.
# For UNIX on the other hand everything is exported by default.
# Until external API is specified explicitly, build STATIC for WIN32
	add_library(pwrutils SHARED ${libpwrutils_STAT_SRC} ${INCS})
else(WIN32)
	add_library(pwrutils SHARED ${libpwrutils_STAT_SRC} ${INCS})
endif(WIN32)
target_link_libraries(pwrutils ${LIBS} )
set_target_properties(pwrutils PROPERTIES
	VERSION "${pwrutils_ver_major}.${pwrutils_ver_minor}"
	SOVERSION ${pwrutils_ver_major})

	install(TARGETS pwrutils
		RUNTIME DESTINATION bin
		ARCHIVE DESTINATION lib
		LIBRARY DESTINATION lib)

	install(
		DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		DESTINATION include
		FILES_MATCHING PATTERN "*.h"
		PATTERN ".svn" EXCLUDE
		PATTERN "bin" EXCLUDE
		PATTERN "build" EXCLUDE
		PATTERN "CMake*" EXCLUDE
	)
	install(
		DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/
		DESTINATION include
		FILES_MATCHING PATTERN "version.h"
	)

