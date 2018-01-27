PROJECT( test )

set(LIBS ${Boost_LIBRARIES})

find_package(LibXML++ REQUIRED QUIET)
include_directories(${LibXML++_INCLUDE_DIRS})
link_directories(${LibXML++_LIBRARY_DIRS})
set(LIBS ${LIBS} ${LibXML++_LIBRARIES})

find_package(ANTLR REQUIRED QUIET)
include_directories(${ANTLR_INCLUDE_DIR})
set(LIBS ${LIBS} ${ANTLR_LIBRARY})

find_package(Loki REQUIRED QUIET)
set(LIBS ${LIBS} loki)

include_directories( ${CMAKE_SOURCE_DIR} )

add_definitions(-DLIBWCCL_TEST_DATA_DIR="${PROJECT_SOURCE_DIR}/")

add_executable(tests
	main.cpp
	optest.cpp

	ann_op.cpp
	conditional.cpp
	constant.cpp
	context.cpp
	datadriven.cpp
	datarule.cpp
	getlemmas.cpp
	getsymbols.cpp
	getsymbolsinrange.cpp
	getorth.cpp
	isempty.cpp
	logicalpredicates.cpp
	mark.cpp
	match.cpp
	position.cpp
	positionpredicates.cpp
	regex.cpp
	relativeposition.cpp
	strsetfunctions.cpp
	values.cpp
	varaccess.cpp
	vargetter.cpp
	variables.cpp
	varsetter.cpp	
)

target_link_libraries ( tests wccl ${LIBS}  )
# Newer LibXML++ requires c++11
if(NOT ${LibXML++_PKGCONF_VERSION} VERSION_LESS 2.40)
	set_target_properties(tests PROPERTIES
		CXX_STANDARD 11)
endif(NOT ${LibXML++_PKGCONF_VERSION} VERSION_LESS 2.40)

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

add_custom_target(test tests)
add_dependencies(test mwtest)
add_custom_target(test-verbose test --log_level=message)
