########## libcorpus2 ###############
PROJECT(corpus2)

if(NOT LIBCORPUS2_SRC_DATA_DIR)
	message(FATAL_ERROR "There is no data for libcorpus2")
endif(NOT LIBCORPUS2_SRC_DATA_DIR)

set(LIBCORPUS2_PATH_SEPARATOR ";")
set(LIBCORPUS2_DATA_DIR
	".;${CMAKE_INSTALL_PREFIX}/${LIBCORPUS2_INSTALL_DATA_DIR}")
set(LIBCORPUS2_VERSION
	"${corpus2_ver_major}.${corpus2_ver_minor}.${corpus2_ver_patch}")

configure_file(config_d.in include/libcorpus2/config_d.h)
configure_file(version.in include/libcorpus2/version.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include/)
add_definitions(-DHAVE_CONFIG_D_H)
add_definitions(-DHAVE_VERSION_H)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include/)

find_package(ICU REQUIRED QUIET)
include_directories(${ICU_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIR})
link_directories(${ICU_LIBRARY_DIRS})
set(LIBS ${LIBS} icuuc icuio)

find_package(Loki REQUIRED QUIET)
set(LIBS ${LIBS} loki)

find_package(LibXML++ REQUIRED QUIET)
include_directories(${LibXML++_INCLUDE_DIRS})
link_directories(${LibXML++_LIBRARY_DIRS})
set(LIBS ${LIBS} ${LibXML++_LIBRARIES})

link_directories(${Boost_LIBRARY_DIRS})
set(LIBS ${LIBS} ${Boost_LIBRARIES})

SET(libcorpus2_STAT_SRC
	ann/annotatedsentence.cpp
	ann/channel.cpp
	ann/iob.cpp 
	ann/view.cpp
	chunk.cpp
	exception.cpp
	lexeme.cpp
	sentence.cpp
	tag.cpp
	tagging.cpp
	tagset.cpp
	tagsetmanager.cpp
	tagsetparser.cpp
	token.cpp
	tokenmetadata.cpp
        io/compressor.cpp
        io/boostcompressor.cpp
        io/cclgzreader.cpp
        io/cclgzwriter.cpp
	io/cclreader.cpp
	io/cclwriter.cpp
	io/conllwriter.cpp
	io/helpers.cpp
	io/fastxces.cpp
	io/iob-chan.cpp
	io/linewriter.cpp
	io/nonewriter.cpp
	io/orthwriter.cpp
	io/pathwriter.cpp
	io/plainreader.cpp
	io/plainwriter.cpp
	io/premorphwriter.cpp
	io/reader.cpp
	io/rft.cpp
	io/sax.cpp
	io/statwriter.cpp
	io/writer.cpp
	io/xces.cpp
	io/xcescommon.cpp
	io/xcesreader.cpp
	io/xcesvalidate.cpp
	io/xceswriter.cpp
	io/xmlreader.cpp
	io/xmlwriter.cpp
	util/ioformat-options.cpp
	util/settings.cpp
	util/symboldictionary.cpp
	util/tokentimer.cpp
	guesser/guesser.cpp
)

file(GLOB_RECURSE INCS "*.h" "*.tpp")

if(WIN32)
# For DLLs on Windows (aka SHARED libraries) you have to explicitly
# specify the external API of the library. Nothing is exported
# by default.
# For UNIX on the other hand everything is exported by default.
# Until external API is specified explicitly, build STATIC for WIN32
	add_library(corpus2 SHARED ${libcorpus2_STAT_SRC} ${INCS})
else(WIN32)
	add_library(corpus2 SHARED ${libcorpus2_STAT_SRC} ${INCS})
endif(WIN32)
target_link_libraries(corpus2 pwrutils ${LIBS})
set_target_properties(corpus2 PROPERTIES
	VERSION "${corpus2_ver_major}.${corpus2_ver_minor}"
	SOVERSION ${corpus2_ver_major})
# Newer libxml++ requires compilation with c++11
if(NOT ${LibXML++_PKGCONF_VERSION} VERSION_LESS 2.40)
	set_target_properties(corpus2 PROPERTIES CXX_STANDARD 11)
endif(NOT ${LibXML++_PKGCONF_VERSION} VERSION_LESS 2.40)

	install(TARGETS corpus2
		RUNTIME DESTINATION bin
		ARCHIVE DESTINATION lib
		LIBRARY DESTINATION lib)

	install(
		DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		DESTINATION include
		FILES_MATCHING PATTERN "*.h"
		PATTERN "*.tpp"
		PATTERN ".svn" EXCLUDE
		PATTERN "bin" EXCLUDE
		PATTERN "build" EXCLUDE
		PATTERN "CMake*" EXCLUDE
	)
	install(
		DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/
		DESTINATION include
		FILES_MATCHING PATTERN "version.h"
		PATTERN "config_d.h"
	)
	install(
		DIRECTORY ${LIBCORPUS2_SRC_DATA_DIR}/
		DESTINATION ${LIBCORPUS2_INSTALL_DATA_DIR}
		FILES_MATCHING PATTERN "*.ini"
		PATTERN "*.tagset"
		PATTERN ".svn" EXCLUDE
		PATTERN "local/*" EXCLUDE
	)

add_executable(test_guesser guesser/test.cpp)
target_link_libraries(test_guesser pwrutils ${LIBS} corpus2)

add_executable(auto_test_guesser guesser/auto_test.cpp)
target_link_libraries(auto_test_guesser pwrutils ${LIBS} corpus2)
