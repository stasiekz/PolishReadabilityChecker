PROJECT(WcrftReader)
cmake_minimum_required(VERSION 2.8.0)
set(libwcrftreader_ver_major 1)
set(libwcrftreader_ver_major 0)

find_package(Corpus2 1.7.2 REQUIRED)
include_directories(${Corpus2_INCLUDE_DIR})
link_directories(${Corpus2_LIBRARY_DIR})
set(LIBS ${LIBS} ${Corpus2_LIBRARY})

set(LIBS ${LIBS} wcrft)

add_library(corpus2_wcrftreader SHARED wcrftreader.cpp)
set_target_properties(corpus2_wcrftreader PROPERTIES
	VERSION "${libwcrftreader_ver_major}.${libwcrftreader_ver_major}"
	SOVERSION ${libwcrftreader_ver_major})
target_link_libraries(corpus2_wcrftreader ${LIBS})

if(UNIX)
	install(TARGETS corpus2_wcrftreader LIBRARY DESTINATION lib)
	install(
		DIRECTORY .
		DESTINATION include/libwcrftreader
		FILES_MATCHING PATTERN "*.h"
	)
endif(UNIX)
