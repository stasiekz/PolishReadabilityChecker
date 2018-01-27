PROJECT( wccl-features )

find_package(Libedit)
if (Libedit_FOUND)
	message(STATUS "Building with libedit")
	add_definitions( -DHAVE_LIBEDIT )
	set(LIBS ${LIBS} ${Libedit_LIBRARIES})
endif (Libedit_FOUND)

find_package(LibXML++ REQUIRED)
include_directories(${LibXML++_INCLUDE_DIRS})
link_directories(${LibXML++_LIBRARY_DIRS})
set(LIBS ${LIBS} ${LibXML++_LIBRARIES})

find_package(Loki REQUIRED QUIET)
set(LIBS ${LIBS} loki)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${wccl_BINARY_DIR}/include)
include_directories(${Boost_INCLUDE_DIR})
include_directories(${ICU_INCLUDE_DIR})
include_directories(${Corpus2_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

add_executable(wccl-features wccl-features.cpp)
target_link_libraries (wccl-features wccl ${Boost_LIBRARIES} antlr ${LIBS})
add_executable(wccl-run wccl-run.cpp stdopts.cpp)
target_link_libraries (wccl-run wccl ${Boost_LIBRARIES} antlr ${LIBS})
add_executable(wccl-rules wccl-rules.cpp)
target_link_libraries (wccl-rules wccl ${Boost_LIBRARIES} antlr ${LIBS})
add_executable(wccl-parser wccl-parser.cpp)
target_link_libraries (wccl-parser wccl ${Boost_LIBRARIES} antlr ${LIBS})
# Newer LibXML++ requires c++11
if(NOT ${LibXML++_PKGCONF_VERSION} VERSION_LESS 2.40)
	set_target_properties(wccl-features PROPERTIES
		CXX_STANDARD 11)
	set_target_properties(wccl-run PROPERTIES
		CXX_STANDARD 11)
	set_target_properties(wccl-rules PROPERTIES
		CXX_STANDARD 11)
	set_target_properties(wccl-parser PROPERTIES
		CXX_STANDARD 11)
endif(NOT ${LibXML++_PKGCONF_VERSION} VERSION_LESS 2.40)

	install(TARGETS wccl-features wccl-run wccl-rules wccl-parser
		RUNTIME DESTINATION bin
	)
