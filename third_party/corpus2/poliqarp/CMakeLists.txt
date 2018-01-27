PROJECT(corpus2_poliqarpreader)
cmake_minimum_required(VERSION 2.8.0)
set(c2pq_ver_major 1)
set(c2pq_ver_minor 0)
include_directories(${PoliqarpLibrary_SOURCE_DIR}/sakura)
include_directories(${PoliqarpLibrary_SOURCE_DIR})
include_directories(${PoliqarpLibrary_BINARY_DIR}/sakura)
include_directories(${PoliqarpLibrary_BINARY_DIR})

add_library(corpus2_poliqarpreader SHARED pqclient.cpp pqreader.cpp)
set_target_properties(corpus2_poliqarpreader PROPERTIES
	VERSION "${c2pq_ver_major}.${c2pq_ver_minor}"
	SOVERSION ${c2pq_ver_major})
target_link_libraries(corpus2_poliqarpreader poliqarpc2 corpus2)

add_executable(c2pqtest c2pqtest.cpp)
target_link_libraries(c2pqtest poliqarpc2 corpus2_poliqarpreader corpus2 pwrutils )

if(UNIX)
	install(TARGETS corpus2_poliqarpreader LIBRARY DESTINATION lib)
	install(TARGETS c2pqtest RUNTIME DESTINATION bin)
	# ${CMAKE_CURRENT_SOURCE_DIR}
	install(
		DIRECTORY .
		DESTINATION include/corpus2_poliqarpreader
		FILES_MATCHING PATTERN "*.h"
	)
endif(UNIX)
