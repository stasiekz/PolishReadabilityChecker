PROJECT( test )

include_directories( ${CMAKE_SOURCE_DIR} )

add_definitions(-DLIBCORPUS2_TEST_DATA_DIR="${PROJECT_SOURCE_DIR}/")

add_executable( tests
	main.cpp
	ann_basic.cpp
	basic.cpp
	io.cpp
	ioann.cpp
	tag_split.cpp
	tagset_parse.cpp
	tokenmetadata.cpp
)

target_link_libraries ( tests corpus2 pwrutils ${Boost_LIBRARIES} ${LibXML++_LIBRARIES})

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

include_directories(${LibXML++_INCLUDE_DIR})
link_directories(${LibXML++_LIBRARY_DIR})

include_directories(${LibXML++Config_INCLUDE_DIR})

include_directories(${Glibmm_INCLUDE_DIR})
link_directories(${Glibmm_LIBRARY})

include_directories(${GlibmmConfig_INCLUDE_DIR})

include_directories(${Glib_INCLUDE_DIR})
link_directories(${Glib_LIBRARY})

include_directories(${GlibConfig_INCLUDE_DIR})

if(NOT ${LibXML++_PKGCONF_VERSION} VERSION_LESS 2.40)
        set_target_properties(tests PROPERTIES CXX_STANDARD 11)
endif(NOT ${LibXML++_PKGCONF_VERSION} VERSION_LESS 2.40)

add_custom_target(test tests)
