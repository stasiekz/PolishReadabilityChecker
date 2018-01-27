PROJECT(MWEReader)
cmake_minimum_required(VERSION 2.8.0)
set(libmwereader_major 0)
set(libmwereader_minor 1)


find_package(Corpus2 1.0.9 REQUIRED)
include_directories(${Corpus2_INCLUDE_DIR})
link_directories(${Corpus2_LIBRARY_DIR})
set(LIBS ${LIBS} ${Corpus2_LIBRARY})

find_package(PwrUtils 1.0.1 REQUIRED)
include_directories(${PwrUtils_INCLUDE_DIR})
set(LIBS ${LIBS} ${PwrUtils_LIBRARY})

link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIR})
set(LIBS ${LIBS} ${Boost_LIBRARIES})

find_package(ICU REQUIRED)
include_directories(${ICU_INCLUDE_DIR})
set(LIBS ${LIBS} ${ICU_LIBRARIES} ${ICU_I18N_LIBRARIES})

find_package(ANTLR REQUIRED)
include_directories(${ANTLR_INCLUDE_DIR})

find_package(LibXML++ REQUIRED QUIET)
include_directories(${LibXML++_INCLUDE_DIRS})
link_directories(${LibXML++_LIBRARY_DIRS})
set(LIBS ${LIBS} ${LibXML++_LIBRARIES})

if(UNIX)
	set(LIBS ${LIBS} wccl antlr-pic)
else()
	set(LIBS ${LIBS} wccl antlr)
endif()

add_library(corpus2_mwereader SHARED mwereader.cpp mwe.cpp mweparser.cpp)

target_link_libraries(corpus2_mwereader ${LIBS})

set_target_properties(corpus2_mwereader PROPERTIES
	VERSION "${libmwereader_major}.${libmwereader_minor}"
	SOVERSION ${libmwereader_major})
# Newer LibXML++ requires c++11
if(NOT ${LibXML++_PKGCONF_VERSION} VERSION_LESS 2.40)
	set_target_properties(corpus2_mwereader PROPERTIES
		CXX_STANDARD 11)
endif()


	install(TARGETS corpus2_mwereader
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		RUNTIME DESTINATION bin
	)
	#install(TARGETS c2pqtest RUNTIME DESTINATION bin)
    install(
		DIRECTORY ./
		DESTINATION include/libmwereader
		FILES_MATCHING PATTERN "*.h"
	)

add_subdirectory(tests)
