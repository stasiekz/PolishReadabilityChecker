PROJECT(corpus2_whole)

find_package(LibXML++ REQUIRED QUIET)
include_directories(${LibXML++_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${ICU_LIBRARY_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
include_directories(${ICU_INCLUDE_DIR})
link_directories(${LibXML++_LIBRARY_DIRS})
set(LIBS ${LIBS} ${LibXML++_LIBRARIES})

SET(libcorpus2_whole_SRC
	corpus.cpp
	document.cpp
	relation.cpp
	io/reader_i.h
	io/relreader.cpp
	io/relwriter.cpp
	io/cclrelreader.cpp
	io/documentcorpusreader.cpp
	io/corpusreader.cpp
	io/documentreader.cpp
	io/basereader.cpp
	io/baserelreader.cpp

)

if(CORPUS2_BUILD_POLIQARP)
	SET(libcorpus2_whole_SRC ${libcorpus2_whole_SRC}
		io/poliqarpdocumentreader.cpp
		io/poliqarpcorpusreader.cpp
	)
endif(CORPUS2_BUILD_POLIQARP)

file(GLOB_RECURSE INCS "*.h")

if(WIN32)
	add_library(corpus2_whole SHARED ${libcorpus2_whole_SRC} ${INCS})
else(WIN32)
	add_library(corpus2_whole SHARED ${libcorpus2_whole_SRC} ${INCS})
endif(WIN32)
target_link_libraries(corpus2_whole corpus2 ${LIBS})
# Newer libxml++ requires compilation with c++11
if(NOT ${LibXML++_PKGCONF_VERSION} VERSION_LESS 2.40)
	set_target_properties(corpus2_whole PROPERTIES CXX_STANDARD 11)
endif(NOT ${LibXML++_PKGCONF_VERSION} VERSION_LESS 2.40)

if(UNIX)
	install(TARGETS corpus2_whole
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
endif(UNIX)

