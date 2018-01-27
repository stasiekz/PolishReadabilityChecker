PROJECT( mwtest )

include_directories( ${CMAKE_SOURCE_DIR} )

add_definitions(-DMWE_READER_TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
#MESSAGE(status ${CMAKE_CURRENT_SOURCE_DIR})
add_executable( mwtests
	main.cpp

	mwefunctional.cpp
)
target_link_libraries(mwtests corpus2_mwereader ${LIBS} antlr)
add_custom_target(mwtest mwtests --log_level=message)
# Newer LibXML++ requires c++11
if(NOT ${LibXML++_PKGCONF_VERSION} VERSION_LESS 2.40)
	set_target_properties(mwtests PROPERTIES
		CXX_STANDARD 11)
endif(NOT ${LibXML++_PKGCONF_VERSION} VERSION_LESS 2.40)
